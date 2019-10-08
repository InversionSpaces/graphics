#include "linalg.hpp"
#include "coordscounter.hpp"
#include "xwindow.hpp"

struct line_t
{
    realcoords_t dots[2];
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
