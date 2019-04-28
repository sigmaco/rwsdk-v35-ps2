// RwRf3DynKeyGen.h: interface for the RwRf3DynKeyGen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RWRF3DYNKEYGEN_H__058FBC2C_9114_4127_9B84_5B466C9FDC86__INCLUDED_)
#define AFX_RWRF3DYNKEYGEN_H__058FBC2C_9114_4127_9B84_5B466C9FDC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RwMIAnimProp.h"

#include "RwExpUtils.h"

template<class KeyType, class Interp, class Deviation>
class RwRf3DynKeyGen  
{
public:
    RwRf3DynKeyGen() {}
    virtual ~RwRf3DynKeyGen() {}

    void    AddKey(RwCommReal time, const KeyType &value, bool bOrigKeyFrame)
    {
        m_vKeys.push_back( RwMIAnimPropKey<KeyType>(time, value) );
        m_vOrigKeys.push_back(bOrigKeyFrame);
    }

    // Apply the operation
    RwCommError Apply(RwCommReal tolerance)
    {
        int start, end;

        for (start = end = 0;; start = end)
        {
            // Find two marked keyframes
            while ((end == start) || (!m_vOrigKeys[end]))
            {
                end++;
                if (end >= m_vKeys.size())
                {
                    return RWCOMM_SUCCESS;
                }
            }

            // Check the error at intermediate keys
            CheckKeyDeviation(start, end, tolerance);
        }
    }

    // Get the resulting key times
    RwCommError GetKeyTimes(vector<RwCommReal> &vKeyTimes)
    {
        for (int i = 0; i < m_vKeys.size(); i++)
        {
            if (m_vOrigKeys[i])
            {
                vKeyTimes.push_back(m_vKeys[i].m_time);
            }
        }

        return RWCOMM_SUCCESS;
    }

    RwCommError GetKeys(vector< RwMIAnimPropKey<KeyType> > &vKeys)
    {
        for (int i = 0; i < m_vKeys.size(); i++)
        {
            if (m_vOrigKeys[i])
            {
                vKeys.push_back(m_vKeys[i]);
            }
        }

        return RWCOMM_SUCCESS;
    }

    unsigned GetNumKeys()
    {
        return m_vKeys.size();
    }

protected:

    void CheckKeyDeviation(int start, int end, RwCommReal tolerance)
    {
        // Make sure we have a middle key
        if (end - start > 1)
        {
            Interp interp;
            Deviation dev;

            int middle = (start + end)/2;

            RwCommReal time = m_vKeys[middle].m_time;
            RwCommReal factor = (time - m_vKeys[start].m_time)/(m_vKeys[end].m_time - m_vKeys[start].m_time);

            // Calculate the interpolated key
            KeyType interpKey = interp(factor, m_vKeys[start].m_value, m_vKeys[end].m_value);

            if (dev(m_vKeys[middle].m_value, interpKey, m_vKeys[start], m_vKeys[end]) > tolerance)
            {
                m_vOrigKeys[middle] = true;
            }

            // Recurse on both sections
            CheckKeyDeviation(start, middle, tolerance);
            CheckKeyDeviation(middle, end, tolerance);
        }
    }

protected:

    vector< RwMIAnimPropKey<KeyType> >    m_vKeys;
    vector<bool>                    m_vOrigKeys;

};


// Predefined deviation methods

template<class KeyType>
struct RwRf3NumDeviation
{
    RwCommReal operator() (const KeyType &v1, const KeyType &v2,
        const RwMIAnimPropKey<KeyType> &k1, const RwMIAnimPropKey<KeyType> &k2)
    {
        RwCommReal dev = 0.0;

        RwCommReal choordDX = (k2.m_time - k1.m_time);
        RwCommReal choordDY = (k2.m_value - k1.m_value);
        RwCommReal choordLen = sqrt(choordDX*choordDX + choordDY*choordDY);

        if (choordLen > 0.0)
        {
            dev = fabs(v2 - v1)/choordLen;
        }
        return dev;
    }
};

template<class KeyType>
struct RwRf3VecDeviation
{
    RwCommReal operator() (const KeyType &v1, const KeyType &v2,
        const RwMIAnimPropKey<KeyType> &k1, const RwMIAnimPropKey<KeyType> &k2)
    {
        RwCommReal dev = 0.0;
        KeyType devs;
        RwCommReal choordDX = (k2.m_time - k1.m_time);

        // Compute a psuedo chordal deviation for all vector components
        for (int i = 0; i < v1.size(); i++)
        {
            RwCommReal choordDY = (k2.m_value[i] - k1.m_value[i]);
            RwCommReal choordLen = sqrt(choordDX*choordDX + choordDY*choordDY);

            if (choordLen > 0)
            {
                devs[i] = fabs(v2[i] - v1[i])/choordLen;
            }
        }
        
        // Find the maximum of all deviations
        for (i = 0; i < v1.size(); i++)
        {
            if (dev < devs[i])
            {
                dev = devs[i];
            }
        }
        
        return dev;
    }
};

struct RwRf3MatDeviation
{
    RwCommReal operator() (const RwCommMatrix &M1, const RwCommMatrix &M2,
        const RwMIAnimPropKey<RwCommMatrix> &k1, const RwMIAnimPropKey<RwCommMatrix> &k2)
    {
        RwCommReal d1,d2;
        RwRf3VecDeviation<RwCommVec3> pdev;
        RwRf3VecDeviation<RwCommVec4> qdev;

        // Extract quats 
        RwCommVec4 q1 = RwExpQuatFromMatrix(M1);
        RwCommVec4 q2 = RwExpQuatFromMatrix(M2);
        RwCommVec4 qk1 = RwExpQuatFromMatrix(k1.m_value);
        RwCommVec4 qk2 = RwExpQuatFromMatrix(k2.m_value);

        d1 = qdev(q1, q2, RwMIAnimPropKey<RwCommVec4>(k1.m_time, qk1), RwMIAnimPropKey<RwCommVec4>(k2.m_time, qk2));
        
        // Get positions
        RwCommVec3 p1 = RwCommVec3(M1[3][0], M1[3][1], M1[3][2]);
        RwCommVec3 p2 = RwCommVec3(M2[3][0], M2[3][1], M2[3][2]);
        RwCommVec3 kp1 = RwCommVec3(k1.m_value[3][0], k1.m_value[3][1], k1.m_value[3][2]);
        RwCommVec3 kp2 = RwCommVec3(k2.m_value[3][0], k2.m_value[3][1], k2.m_value[3][2]);

        d2 = pdev(p1, p2, RwMIAnimPropKey<RwCommVec3>(k1.m_time, kp1), RwMIAnimPropKey<RwCommVec3>(k2.m_time, kp2));

        return max(d1,d2);
    }
};

// predefined interpolation methods
template<class T>
struct RwRf3VecLerp
{
    vector<T> operator() (RwCommReal factor, const vector<T> &v1, const vector<T> &v2) const
    {
        int md = min(v1.size(), v2.size());
        vector<T> v(md);

        for (int i = 0; i < md; i++)
        {
            v[i] = v1[i]*(1.0 - factor) + v2[i]*factor;
        }

        return v;
    } 
};


struct RwRf3MatInterp
{
    RwCommMatrix operator() (RwCommReal factor, const RwCommMatrix &M1, const RwCommMatrix &M2) const
    {
        return RwExpSlerp(factor, M1, M2);
    }    
};


template<class KeyType>
RwCommError RwRf3BuildKeyTimes(vector<RwCommReal> &vKeyTimes, const vector< RwMIAnimPropKey<KeyType> > &vKeys)
{
    for (int i = 0; i < vKeys.size(); i++)
    {
        vKeyTimes.push_back(vKeys[i].m_time);
    }
    return RWCOMM_SUCCESS;
}

typedef RwRf3DynKeyGen<RwCommReal, RwCommLerp<RwCommReal>, RwRf3NumDeviation<RwCommReal> > RwRf3RealAnimation;
typedef RwRf3DynKeyGen< vector<RwCommReal>, RwRf3VecLerp< RwCommReal >, RwRf3VecDeviation<vector<RwCommReal> > > RwRf3VecAnimation;
typedef RwRf3DynKeyGen<RwCommMatrix, RwRf3MatInterp, RwRf3MatDeviation> RwRf3MatAnimation;

#endif // !defined(AFX_RWRF3DYNKEYGEN_H__058FBC2C_9114_4127_9B84_5B466C9FDC86__INCLUDED_)
