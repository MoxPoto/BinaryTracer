#include "Vector3.h"
#include "Object.h"
#include "Triangle.h"
#include "Ray.h"

#include "GarrysMod/Lua/Interface.h"
#include "../MainTracer.h"

#include <memory>

constexpr float kEpsilon = 1e-8;
constexpr double MAX_DOUBLE = std::numeric_limits<double>::max();

#define MOLLER_TRUMBORE

bool rayTriangleIntersect(
    const Tracer::Vector3& orig, const Tracer::Vector3& dir,
    const Tracer::Vector3& v0, const Tracer::Vector3& v1, const Tracer::Vector3& v2,
    double& t, double& u, double& v)
{
#ifdef MOLLER_TRUMBORE 
    Tracer::Vector3 v0v1 = v1 - v0;
    Tracer::Vector3 v0v2 = v2 - v0;
    Tracer::Vector3 pvec = dir.cross(v0v2);
    float det = (float)v0v1.dot(pvec);
#ifdef CULLING 
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < kEpsilon) return false;
#else 
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false;
#endif 
    float invDet = 1 / det;

    Tracer::Vector3 tvec = orig - v0;
    u = tvec.dot(pvec) * (double)invDet;
    if (u < 0 || u > 1) return false;

    Tracer::Vector3 qvec = tvec.cross(v0v1);
    v = dir.dot(qvec) * (double)invDet;
    if (v < 0 || u + v > 1) return false;

    t = v0v2.dot(qvec) * (double)invDet;

    return true;
#else 
    // compute plane's normal
    Tracer::Vector3 v0v1 = v1 - v0;
    Tracer::Vector3 v0v2 = v2 - v0;
    // no need to normalize
    Tracer::Vector3 N = v0v1.cross(v0v2); // N 
    float denom = N.dot(N);

    // Step 1: finding P

    // check if ray and plane are parallel ?
    float NdotRayDirection = N.dot(dir);
    if (fabs(NdotRayDirection) < kEpsilon) // almost 0 
        return false; // they are parallel so they don't intersect ! 

    // compute d parameter using equation 2
    float d = N.dot(v0);

    // compute t (equation 3)
    t = (N.dot(orig) + d) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind 

    // compute the intersection point using equation 1
    Tracer::Vector3 P = orig + (dir * t);

    // Step 2: inside-outside test
    Tracer::Vector3 C(0, 0, 0); // vector perpendicular to triangle's plane 

    // edge 0
    Tracer::Vector3 edge0 = v1 - v0;
    Tracer::Vector3  vp0 = P - v0;
    C = edge0.cross(vp0);
    if (N.dot(C) < 0) return false; // P is on the right side 

    // edge 1
    Tracer::Vector3  edge1 = v2 - v1;
    Tracer::Vector3  vp1 = P - v1;
    C = edge1.cross(vp1);
    if ((u = N.dot(C)) < 0)  return false; // P is on the right side 

    // edge 2
    Tracer::Vector3  edge2 = v0 - v2;
    Tracer::Vector3  vp2 = P - v2;
    C = edge2.cross(vp2);
    if ((v = N.dot(C)) < 0) return false; // P is on the right side; 

    u /= denom;
    v /= denom;

    return true; // this ray hits the triangle 
#endif 
}

/*
bool traceEnt(const Vector3& origin, const Vector3& direction, HitData& out, const Entity* ent, double& tMin, double& uMin, double& vMin, int hitVert[3])
{
    // localise ray to entity's transform
    Vector3 adjOrig = (origin - ent->pos).getRotated(ent->invAng);
    Vector3 adjDir = direction.getRotated(ent->invAng);

    bool hit = false;
    double t = MAX_DOUBLE, u, v;

    // iterate sub meshes
    for (int i = 0; i < ent->meshes.size(); i++) {
        const Mesh* mesh = &ent->meshes[i];

        // iterate tris
        for (int j = 0; j < mesh->verts.size(); j += 3) {
            if (triIntersect(
                adjOrig, adjDir,
                mesh->verts[j].pos,
                mesh->verts[j + 1].pos,
                mesh->verts[j + 2].pos,
                t, u, v
            ) && t < tMin) {
                // assign tri hit if it's closest
                tMin = t;
                uMin = u;
                vMin = v;
                hitVert[1] = i;
                hitVert[2] = j;
                hit = true;
            }
        }
    }

    return hit;
}

*/

bool onedDebug = false;

namespace Tracer {

    Vector3 SUN_DIR = Vector3(-0.234490, 0.441133, 0.866266);

    Ray::Ray() {
        ignoreID = -1; // Set the ignore ID to -1 at first
    }

    // Performs a ray-cast TODO: debug and find the thing that keeps causing it to not correctly iterate
    TraceResult* Ray::cast() {
        TraceResult* result = new TraceResult;

        result->Hit = false;


        double tMin = MAX_DOUBLE;

        double t = 0;
        double u = 0;
        double v = 0;

        for (std::shared_ptr<Object> theObject : AllObjects) {
            if (theObject->objectID == ignoreID) {
                continue;
            }

            for (const Triangle& theTri : theObject->tris) {

                if (rayTriangleIntersect(orig, dir, theTri.v0, theTri.v1, theTri.v2, t, u, v) && t < tMin) {
                    tMin = t;

                    result->Hit = true;
                    result->Object = theObject;
                    result->t = t;
                    result->u = u;
                    result->v = v;
                    result->HitColor = theTri.color;
                    result->HitPos = orig + (dir * t); // t means the time of the ray, aka how much it traveled

                    //hitNormal = (1 - uv.x - uv.y) * n0 + uv.x * n1 + uv.y * n2; 

                    //result->HitNormal = theTri.n1 * (1.0 - u - v) + theTri.n2 * u + theTri.n3 * v;
                    
                    result->HitNormal = (theTri.n1);
                    
                    /*
                    Vector3 theU = (theTri.v1 - theTri.v0);
                    Vector3 theV = (theTri.v2 - theTri.v0);

                    result->HitNormal = -(theV.cross(theU)).getNormalized();
                    */

                    if (onedDebug == false) {
                        
                        luaPrint("u: " + std::to_string(result->u));
                        luaPrint("v: " + std::to_string(result->v));
                        luaPrint("t: " + std::to_string(t));
                        luaPrint("hitnormal calc: " + vectorAsAString(result->HitNormal));
                        luaPrint("n1: " + vectorAsAString(theTri.n1));
                        luaPrint("n2: " + vectorAsAString(theTri.n2));
                        luaPrint("n3: " + vectorAsAString(theTri.n3));
                        

                        onedDebug = true;
                    }

                }
            }
        }

        return result;
    }

}