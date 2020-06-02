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
    Ptr<SIFT> detector = SIFT::create(450);
    unsigned int _L;
    vector<float> _meanvec;     // The mean vector
//    unordered_map<vector<float>, int> _vecmap;

public:
    LSH *_lsh;
    unordered_map<unsigned int, int> _score;
    unordered_map<int, string> _namemap;
    vector<vector<float>> _featureMT;
    /*Constructor */
    explicit Extractor(int numHashes, LSH *lsh, unsigned int L);


    unsigned int compute(vector<string> files);

    unsigned int preprocessing();

    unsigned int query(const String& filePath, unsigned int top_k);

    /*
    * vector1 and vector2 must have the same size, n.
    */
    static vector<float> vecminus(vector<float> vector1, vector<float> vector2, unsigned int size) {
            assert(vector1.size() == size && vector2.size() == size);
            vector<float> result;
            result.reserve(size);
            for (int i = 0; i < size; ++i) {
                    result.push_back((vector1.at(i) - vector2.at(i)));
            }
            return result;
    }

    ~Extractor();
};

#endif //HAVE_OPENCV_XFEATURES2D
#endif //SURF_SRP_EXTRACTOR_H
