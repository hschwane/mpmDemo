

float N(float x)
{
    x = abs(x);
    return (x<0.5f) ? (0.75f - x*x) : ( (x<1.5f) ? (0.5f * (1.5f-x) * (1.5-x) ) : 0 );
}

// quadratic bspline kernel
float kernel(vec2 x_i, vec2 x_p, float gridLength)
{
    float hinv = 1.0f / gridLength;
    return N(hinv * (x_p.x - x_i.x)) + N(hinv * (x_p.y - x_i.y));
}

//float N(float x)
//{
//    x = abs(x);
//    return (x<1.0f) ? ( 1.0f/2.0f *x*x*x - x*x + 2.0f/3.0f) : ( (x<2.0f) ? (-1.0f/6.0f * x*x*x + x*x - 2*x + 4.0f/3.0f) : 0 );
//}
//
//// cubic bspline kernel
//float kernel(vec2 x_i, vec2 x_p, float gridLength)
//{
//    float hinv = 1.0f / gridLength;
//    return N(hinv * (x_p.x - x_i.x)) + N(hinv * (x_p.y - x_i.y));
//}