#include "linalg.hpp"
#include "coordscounter.hpp"
#include "xwindow.hpp"

struct line_t
{
    realcoords_t dots[2];
};

struct triangle_t
{
    realcoords_t dots[3];
};

class Drawyer
{
private:
    CoordsCounter *cc;
    XWindow *xw;
public:
    Drawyer()
    {
        xw = new XWindow();
        cc = new CoordsCounter(xw->resolution());
    }
    
    void draw(const line_t& line, const bgracolor_t& color)
    {
        pixelcoords_t start = cc->real2pixel(line.dots[0]);
        pixelcoords_t end 	= cc->real2pixel(line.dots[1]);
        
        bool xORy = fabs(start.x - end.x) > fabs(start.y - end.y);
        
        if (( xORy && (start.x > end.x)) ||
            (!xORy && (start.y > end.y)))
            std::swap(start, end);
            
        //std::cout << "Start: x[" << start.x << "] y[" << start.y << "]" << std::endl;
        //std::cout << "End: x[" << end.x << "] y[" << end.y << "]" << std::endl;
        
        uint16_t iterStart 	= 0;
        uint16_t iterEnd 	= 0;
        uint16_t initial	= 0;
        
        float tg = 0;
        
        if (xORy) {
            initial 	= start.y;
            
            iterStart 	= start.x;
            iterEnd 	= end.x;
            
            tg = float(end.y - start.y) / float(end.x - start.x);
        }
        else {
            initial 	= start.x;
            
            iterStart 	= start.y;
            iterEnd 	= end.y;
            
            tg = float(end.x - start.x) / float(end.y - start.y);
        }
        
        //std::cout << "Initial: " << initial << std::endl;
        //std::cout << "IterStart: " << iterStart << " IterEnd: " << iterEnd << std::endl;
        //std::cout << "TG: " << tg << std::endl;
        
        for (uint16_t i = iterStart; i <= iterEnd; ++i) {
            uint16_t j = initial + std::lround(tg * (i - iterStart));
            //std::cout << i << " " << j << std::endl;
            (*xw)[xORy ? pixelcoords_t({i, j}) : pixelcoords_t({j, i})] = color;
        }
    }
    
    void draw(const triangle_t& triangle, const bgracolor_t& color) 
    {
        screencoords_t r0 = cc->real2screen(triangle.dots[0]);
        screencoords_t r1 = cc->real2screen(triangle.dots[1]);
        screencoords_t r2 = cc->real2screen(triangle.dots[2]);
        
        //std::cout << r0.x << " " << r0.y << std::endl;
        //std::cout << r1.x << " " << r1.y << std::endl;
        //std::cout << r2.x << " " << r2.y << std::endl;
        
        screencoords_t b = r1 - r0;
        screencoords_t c = r2 - r0;
        
        //std::cout << "b " << b.x << " " << b.y << std::endl;
        //std::cout << "c " << c.x << " " << c.y << std::endl;
        
        float det = sarea(b, c);
        
        //std::cout << "det " << det << std::endl;
        
        pixelcoords_t r0p = cc->screen2pixel(r0);
        pixelcoords_t r1p = cc->screen2pixel(r1);
        pixelcoords_t r2p = cc->screen2pixel(r2);
        
        uint16_t xmin = std::min(r0p.x, std::min(r1p.x, r2p.x));
        uint16_t xmax = std::max(r0p.x, std::max(r1p.x, r2p.x));
        
        uint16_t ymin = std::min(r0p.y, std::min(r1p.y, r2p.y));
        uint16_t ymax = std::max(r0p.y, std::max(r1p.y, r2p.y));
        
        //std::cout << xmin << "->" << xmax << std::endl;
        //std::cout << ymin << "->" << ymax << std::endl;
        
        //uint16_t count = 0;
        for (uint16_t x = xmin; x <= xmax; ++x)
            for (uint16_t y = ymin; y <= ymax; ++y) {
                screencoords_t r = cc->pixel2screen({x, y});
                screencoords_t a = r - r0;
                
                float det1 = sarea(a, c);
                float det2 = sarea(b, a);
                float det3 = det - det1 - det2;
                
                float beta 	= det1 / det;
                float gamma = det2 / det;
                float alpha = det3 / det;
                
                //std::cout << x << " " << y << std::endl;
                //std::cout << alpha << " " << beta << " " << gamma << std::endl;
                
                //std::cout << "Bad " << x << " " << y << std::endl;
                if ((alpha > 0) && (beta > 0) && (gamma > 0)) //{ 
                    //std::cout << "Good " << x << " " << y << std::endl;
                    (*xw)[{x, y}] = color;
                    //count++;
                //}
            }
        
        //std::cout << (xmax - xmin) * (ymax - ymin) << " " << count << std::endl;
    }
    
    void flush()
    {
        xw->update();
    }
    
    ~Drawyer()
    {	
        delete xw;
        delete cc;
    }
};
