//
// Created by Junyan Zhang on 5/4/20.
//

#ifndef SURF_SRP_EXTRACTOR_H
#define SURF_SRP_EXTRACTOR_H

#include "iostream"
#include "opencv2/core.hpp"
#include "LSH.h"
#include "srpHash.h"
#include "unordered_map"

#ifdef HAVE_OPENCV_XFEATURES2D
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

struct comparePair
{
        bool operator()(pair<int, int> p1, pair<int, int> p2)
        {
                // if frequencies of two elements are same
                // then the larger number should come first
                if (p1.second == p2.second)
                        return p1.first > p2.first;

                // insert elements in the priority queue on the basis of
                // decreasing order of frequencies
                return p1.second > p2.second;
        }
};

class Extractor {
private:
    int _K;
    Ptr<SIFT> detector = SIFT::create(400);
    unsigned int _L;

public:
    LSH *_lsh;
    unordered_map<int, int> _score;
    /*Constructor */
    explicit Extractor(int numHashes, LSH *lsh, unsigned int L );


    unsigned int computeAndHash(const String& filePath, unsigned int imgID);

    /*
     * result must be a top-k * numOfQuery length array.
     */
    unsigned int query(const String& filePath, unsigned int top_k);
    ~Extractor();
};

#endif //HAVE_OPENCV_XFEATURES2D
#endif //SURF_SRP_EXTRACTOR_H
