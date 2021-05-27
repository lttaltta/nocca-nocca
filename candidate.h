
#ifndef CANDIDATE_H_
#define CANDIDATE_H_

class Candidate {
public:
    int src_y, src_x, dst_y, dst_x;

    Candidate () : src_y(-1), src_x(-1), dst_y(-1), dst_x(-1){}
    Candidate (int src_y_, int src_x_, int dst_y_, int dst_x_) : src_y(src_y_), src_x(src_x_), dst_y(dst_y_), dst_x(dst_x_) {}
    bool operator<(const Candidate& c1) const {
        return
            (this->src_y < c1.src_y ? true :
            (this->src_x < c1.src_x ? true :
            (this->dst_y < c1.dst_y ? true :
            (this->dst_x < c1.dst_x ? true : false))));
    }
};

const int candidate_dy[] = {-1, -1, -1,  0,  0,  1,  1,  1};
const int candidate_dx[] = {-1,  0,  1, -1,  1, -1,  0,  1};


#endif
