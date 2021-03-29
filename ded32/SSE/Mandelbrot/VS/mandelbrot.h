#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <TXLib.h>


// TODO: Inline getters and such?

class MandelbrotDisplay {
public:
    static constexpr unsigned WND_HEIGHT = 600;
    static constexpr unsigned WND_WIDTH = 800;

    typedef RGBQUAD (&screen_t)[WND_HEIGHT][WND_WIDTH];

    MandelbrotDisplay();
    MandelbrotDisplay(unsigned new_iterations, float new_delta, float maxR, float new_centerX, float new_centerY);

    MandelbrotDisplay(const MandelbrotDisplay &other) = default;
    MandelbrotDisplay &operator=(const MandelbrotDisplay &other) = default;

    bool render(screen_t screen, bool perfCnt = false) const;

    void renderLoop();

    void perfCount() const;

    void shift(float distX, float distY);

    void scale(float coeff);

    void setRadius(float maxR);

    void setIterations(unsigned new_iterations);

    inline unsigned getIterations() const {
        return iterations;
    }

    inline float getDelta() const {
        return delta;
    }

    inline float getMaxR2() const {
        return maxR2;
    }

    inline float getCenterX() const {
        return centerX;
    }

    inline float getCenterY() const {
        return centerY;
    }

    void reset();

private:
    unsigned iterations;
    float delta;  // Same for x and y
    float maxR2;
    float centerX, centerY;

};


#endif // MANDELBROT_H
