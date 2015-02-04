// EPOS Elliptic Curve Abstraction Declarations

#ifndef __ec_unified_h
#define __ec_unified_h

#include "component.h"

namespace Implementation {

class EC: public Component
{
public:
    enum {
        OP_MULT = 0xF0
    };

public:
    EC(Channel_t &rx_ch, Channel_t &tx_ch, unsigned char inst_id):
            Component(rx_ch, tx_ch, inst_id) {}

    EC_Point_t mult(EC_Point_t p, unsigned long long k) {
        EC_Point_t q;

        q.x = 0;
        q.y = 0;
        q.z = 0;

        for(int i = 64; i > 0; i++) {
            q = jacobian_double(q);

            if(k & (unsigned long long)(1<<i))
                q = add_jacobian_affine(q, p);
        }

        unsigned long long Z;

        //q.z.inverse();
        Z = q.z;
        Z *= q.z;

        q.x *= Z;
        Z *= q.z;

        q.y *= Z;
        q.z = 1;

        return q;
    }

private:
    EC_Point_t add_jacobian_affine(EC_Point_t a, EC_Point_t b) {
        unsigned long long A;
        unsigned long long B;
        unsigned long long C;
        unsigned long long X;
        unsigned long long Y;
        unsigned long long aux;
        unsigned long long aux2;

        A = a.z;
        A *= a.z;

        B = A;

        A *= b.x;

        B *= a.z;
        B *= b.y;

        C = A;
        C -= a.x;

        B -= a.y;

        X = B;
        X *= B;
        aux = C;
        aux *= C;

        Y = aux;

        aux2 = aux;
        aux *= C;
        aux2 *= 2;
        aux2 *= a.x;
        aux += aux2;
        X -= aux;

        aux = Y;
        Y *= a.x;
        Y -= X;
        Y *= B;
        aux *= a.y;
        aux *= C;
        Y -= aux;

        a.z *= C;

        a.x = X;
        a.y = Y;

        return a;
    }

    EC_Point_t jacobian_double(EC_Point_t p) {
        unsigned long long B;
        unsigned long long C;
        unsigned long long aux;

        C = p.x;
        aux = p.z;
        aux *= p.z;
        C -= aux;
        aux += p.x;
        C *= aux;
        C *= 3;

        p.z *= p.y;
        p.z *= 2;

        p.y *= p.y;
        B = p.y;

        p.y *= p.x;
        p.y *= 4;

        B *= B;
        B *= 8;

        p.x = C;
        p.x *= p.x;
        aux = p.y;
        aux *= 2;
        p.x -= aux;

        p.y -= p.x;
        p.y *= C;
        p.y -= B;

        return p;
    }
};

PROXY_BEGIN(EC)
    EC_Point_t mult(EC_Point_t p, unsigned long long k) {
        return Base::call_r<EC::OP_MULT, EC_Point_t>(p, k);
    }
PROXY_END

HANDLE_BEGIN(EC)
    EC_Point_t mult(EC_Point_t p, unsigned long long k) {
        EC_Point_t result;

        Base::enter_recfg();

        if(_domain == Component_Manager::HARDWARE)
            result = Base::_proxy->mult(p, k);
        else
            result = Base::_comp->mult(p, k);

        Base::leave_recfg();

        return result;
    }
HANDLE_END

AGENT_BEGIN(EC)
    D_CALL_R_2(mult, OP_MULT, EC_Point_t, EC_Point_t, unsigned long long)
AGENT_END

};

DECLARE_RECFG_COMPONENT(EC);

#endif
