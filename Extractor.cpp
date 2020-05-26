//
// Created by Junyan Zhang on 5/4/20.
//

#include "Extractor.h"
#include <algorithm>
#include <random>
#include "cmath"
using namespace std;
//using namespace cv;
//using namespace cv::xfeatures2d;

Extractor::Extractor(int numHashes, LSH *lsh, unsigned int L ) {
        _K = numHashes;
        _lsh = lsh;
        _L = L;
}


unsigned int Extractor::computeAndHash(const String& filePath, unsigned int imgID) {
        // Step 1: Detect the keypoints using SURF Detector
        //        double minHessian = 250;
        //        Ptr<SURF> detector = SURF::create(minHessian);
        Mat srcImg = imread(samples::findFile(filePath), IMREAD_COLOR);
        if (srcImg.empty()) {
                std::cout << "Could not open or find the image!\n" << std::endl;
                exit(-1);
        }
        std::vector<KeyPoint> keypoints;

        detector->detect(srcImg, keypoints);
        int numPoints = keypoints.size();
//        cout << numPoints << endl;

        // Draw keypoints
        //        Mat img_keypoints;
        //        drawKeypoints(_srcImg, keypoints, img_keypoints );
        //
        //        //-- Show detected (drawn) keypoints
        //        imshow("SURF Keypoints", img_keypoints);
        //
        //        waitKey();

        // Step 2: Compute feature
        // Case when NOT enough KeyPoints
        if (numPoints < 400) {
                cout << "No 400 keypoints available" << endl;
                return -1;
        }
        // Case when enough KeyPoints
        // Vector for random selection
//        auto rd = std::random_device {};
//        auto rng = default_random_engine {rd()};
//        vector<int> arr;
//        arr.reserve(numPoints);
//        for (int i = 0; i < numPoints; i++) {
//                arr.push_back(i);
//        }
//        shuffle(arr.begin(), arr.end(), rng);

        // Now compute descriptor (feature) for each KeyPoint.
        vector<KeyPoint> newKP;
        Mat descriptor;
        unsigned int *hashlst = new unsigned int[_L];
//        unsigned int *hashes;
        unsigned int hash;
        vector<float> array;
        for (int x = 0; x < 400; x++) {
                if (x % 100 == 0)
                        cout << "in for loop image id = " << imgID << " x = " << x << endl;
                newKP.clear();
                newKP.push_back(keypoints.at(x));
                detector->compute(srcImg, newKP, descriptor);

                if (descriptor.isContinuous()) {
                        array.clear();
                        array.assign((float*)descriptor.data, (float*)descriptor.data + descriptor.total());
                }
//                for (auto i : array) {
//                        cout << i << ", ";
//                }
//                cout << "\nArray size:" << array.size() << endl;

                // Step 3: Hash each 128-dimensional feature here
                for (int m = 0; m < _L; m++) {      // For lsh, compute each table for each feature
                        srpHash *_srp = new srpHash(128, _K, 1);
                        unsigned int *hashes = _srp->getHash(array, 10);
                        hash = 0;
//                        cout << "srp address: " << _srp << endl;
//                        cout << "hash address: " << hashes << endl;
                        for (int x = 0; x < _srp->_numhashes; x++) {
                                // Convert to an interger
                                hash += hashes[x] * pow(2, (_srp->_numhashes - x - 1));
                        }
                        hashlst[m] = hash;
//                        cout << "Hash value: " << hash << endl;
                        delete(_srp);
                        delete [] hashes;
                }
//                for (int i = 0; i < _L; i++)
//                        cout << hashlst[i] << " ";
//                cout << endl;
                _lsh ->insert(imgID, hashlst);
//                cout << "lsh insert successful" << endl;
        }
//        cout << "Descriptor:\n" << descriptor;
//        cout << "\nDescriptor size:" << descriptor.size() << endl;
//        cout << "Descriptor type:" << descriptor.type() << endl;


        return 0;
}

unsigned int Extractor::query(const String &filePath, unsigned int top_k, unsigned int *result) {
        // Step 1: Detect the keypoints using SURF Detector
        //        double minHessian = 250;
        //        Ptr<SURF> detector = SURF::create(minHessian);
        Mat srcImg = imread(samples::findFile(filePath), IMREAD_COLOR);
        if (srcImg.empty()) {
                std::cout << "Could not open or find the image!\n" << std::endl;
                exit(-1);
        }
        std::vector<KeyPoint> keypoints;

        detector->detect(srcImg, keypoints);
        int numPoints = keypoints.size();

        // Step 2: Compute feature
        // Case when NOT enough KeyPoints
        if (numPoints < 400) {
                cout << "Query No 400 keypoints available" << endl;
                return -1;
        }

        // Case when enough KeyPoints
        vector<KeyPoint> newKP;
        Mat descriptor;
        unsigned int *query = new unsigned int[_L * 400];
        //        unsigned int *hashes;
        unsigned int hash;
        vector<float> array;
        for (int x = 0; x < 400; x++) {
                cout << "in Query for loop x = " << x << endl;
                newKP.clear();
                newKP.push_back(keypoints.at(x));
                detector->compute(srcImg, newKP, descriptor);

                if (descriptor.isContinuous()) {
                        array.clear();
                        array.assign((float*)descriptor.data, (float*)descriptor.data + descriptor.total());
                }
                //                for (auto i : array) {
                //                        cout << i << ", ";
                //                }
                //                cout << "\nArray size:" << array.size() << endl;

                // Step 3: Hash each 128-dimensional feature here
                for (int m = 0; m < _L; m++) {      // For lsh, compute each table for each feature
                        srpHash *_srp = new srpHash(128, _K, 1);
                        unsigned int *hashes = _srp->getHash(array, 400);
                        hash = 0;
                        //                        cout << "srp address: " << _srp << endl;
                        //                        cout << "hash address: " << hashes << endl;
                        for (int x = 0; x < _srp->_numhashes; x++) {
                                // Convert to an interger
                                hash += hashes[x] * pow(2, (_srp->_numhashes - x - 1));
                        }
                        query[x*4 + m] = hash;
//                        cout << "Query insert successful" << endl;
                        delete(_srp);
                }
        }
        //                for (int i = 0; i < _L * _K; i++)
        //                        cout << query[i] << " ";
        _lsh->top_k(400, top_k, query, result);
        delete [] query;
        return 0;
}

Extractor::~Extractor() {

}
