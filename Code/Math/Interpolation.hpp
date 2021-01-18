#ifndef _INTERPOLATION_HPP
#define _INTERPOLATION_HPP 1

template<typename X, typename Y>
Y fn_Interpolate(const X& x1, const X& x2, const Y& y1, const Y& y2, const X& x)
{
    return (((y2 - y1) / (x2 - x1)) * (x - x1)) + y1;
}

template<typename Y>
Y fn_Time_Interpolate(float t1, float t2, const Y& y1, const Y& y2, float t)
{
    return fn_Interpolate<float, Y>(t1, t2, y1, y2, t);
}

#endif // _INTERPOLATION_HPP
