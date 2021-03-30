#include <iostream>
#include <cstring>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <functional>

namespace numbers {
class complex {
    double Re;
    double Im;
public:
    complex(double x = 0, double y = 0) {
        Re = x;
        Im = y;
    }
    explicit complex(const std::string &str) {
        sscanf(str.c_str(), "(%lf,%lf)", &Re, &Im);

    }
    double re() const {
        return Re;
    }
    double im() const {
        return Im;
    }
    double abs2() const {
        return Re * Re + Im * Im;
    }
    double abs() const {
        return sqrt(Re * Re + Im * Im);
    }
    std::string to_string() const {
        std::stringstream ss;
        ss << "(" << std::setprecision(10) << Re << "," << Im << ")";
        return ss.str();
    }
    complex& operator +=(const complex &x) {
        Re += x.re();
        Im += x.im();
        return *this;
    }
    complex& operator -=(const complex &x) {
        Re -= x.re();
        Im -= x.im();
        return *this;
    }
    complex& operator *=(const complex &x) {
        double tmp = Re * x.re() - Im * x.im();
        Im = Re * x.im() + Im * x.re();
        Re = tmp;
        return *this;
    }
    complex operator ~() const {
        complex res = *this;
        res.Im = -Im;
        return res;
    }
    complex& operator /=(const complex &x) {
        *this *= ~complex(x.re(), x.im());
        Im /= x.abs2();
        Re /= x.abs2();
        return *this;
    }
    complex operator -() const {
        complex res;
        res.Re = -Re;
        res.Im = -Im;
        return res;
    }
};
complex operator +(const complex &x1, const complex &x2) {
    complex res = x1;
    res += x2;
    return res;
}
complex operator /(const complex &x1, const complex &x2) {
    complex res = x1;
    res /= x2;
    return res;
}
complex operator -(const complex &x1, const complex &x2) {
    complex res = x1;
    res -= x2;
    return res;
}
complex operator *(const complex &x1, const complex &x2) {
    complex res = x1;
    res *= x2;
    return res;
}

class complex_stack {
    complex *stack_mem;
    size_t stack_size;
public:
    complex_stack() {
        stack_size = 0;
        stack_mem = nullptr;
    }
    complex_stack(size_t n) {
        stack_size = n;
        stack_mem = new complex[n];
    }
    complex_stack(const complex_stack &s) {
        stack_size = s.stack_size;
        stack_mem = new complex[stack_size];
        for (size_t i = 0; i < stack_size; ++i) {
            stack_mem[i] = s.stack_mem[i];
        }
    }
    ~complex_stack() {
        delete[] stack_mem;
    }
    size_t size() const {
        return stack_size;
    }
    const complex& operator [](int i) {
        return stack_mem[i];
    }

    complex_stack operator <<(const complex &z) const {
        complex_stack new_stack(stack_size + 1);
        new_stack.stack_mem[stack_size] = z;
        for (size_t i = 0; i < stack_size; ++i) {
            new_stack.stack_mem[i] = stack_mem[i];
        }
        return new_stack;

    }
    complex_stack operator ~() const {
        complex_stack new_stack(stack_size - 1);
        for (size_t i = 0; i < stack_size - 1; ++i) {
            new_stack.stack_mem[i] = stack_mem[i];
        }
        return new_stack;

    }
    complex operator +() const {
        return stack_mem[stack_size - 1];
    }
    complex_stack& operator =(const complex_stack &s) {
        if (&s != this) {
            stack_size = s.stack_size;
            stack_mem = new complex[stack_size];
            for (size_t i = 0; i < stack_size; ++i) {
                stack_mem[i] = s.stack_mem[i];
            }
        }
        return *this;
    }

};

complex eval(const std::vector<std::string> &args, const complex &z) {
    size_t n = args.size();
    std::map<std::string,
            std::function<complex_stack(complex_stack&, const complex&)> > funcs;
    funcs["+"] = [](complex_stack &st, const complex &z) {
        complex z2 = +st;
        st = ~st;
        complex z1 = +st;
        st = ~st;
        z1 += z2;
        st = st << z1;
        return st;
    };
    funcs["-"] = [](complex_stack &st, const complex &z) {
        complex z2 = +st;
        st = ~st;
        complex z1 = +st;
        st = ~st;
        z1 -= z2;
        st = st << z1;
        return st;
    };
    funcs["*"] = [](complex_stack &st, const complex &z) {
        complex z2 = +st;
        st = ~st;
        complex z1 = +st;
        st = ~st;
        z1 *= z2;
        st = st << z1;
        return st;
    };
    funcs["/"] = [](complex_stack &st, const complex &z) {
        complex z2 = +st;
        st = ~st;
        complex z1 = +st;
        st = ~st;
        z1 /= z2;
        st = st << z1;
        return st;
    };
    funcs["!"] = [](complex_stack &st, const complex &z) {
        st = st << +st;
        return st;
    };
    funcs[";"] = [](complex_stack &st, const complex &z) {
        st = ~st;
        return st;
    };
    funcs["~"] = [](complex_stack &st, const complex &z) {
        complex z1 = +st;
        z1 = ~z1;
        st = ~st;
        st = st << z1;
        return st;
    };
    funcs["#"] = [](complex_stack &st, const complex &z) {
        complex z1 = +st;
        z1 = -z1;
        st = ~st;
        st = st << z1;
        return st;
    };

    complex_stack st;
    for (size_t i = 0; i < n; ++i) {
        if (args[i][0] == '(') {
            st = (st << complex(args[i]));
        } else if (args[i][0] == 'z') {
            st = st << z;
        } else  {
            funcs[args[i]](st, z);
        }
    }
    return st[0];
}
}
