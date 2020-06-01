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


unsigned int Extractor::compute(vector<string> files) {
        // Step 1: Detect the keypoints using SURF Detector
        //        double minHessian = 250;
        //        Ptr<SURF> detector = SURF::create(minHessian);
        unsigned int id = 0;
        for (auto filePath : files) {
                _namemap[id] = filePath;

                Mat srcImg = imread(samples::findFile(filePath), IMREAD_COLOR);
                if (srcImg.empty()) {
                        std::cout << "Could not open or find the image!\n" << std::endl;
                        exit(-1);
                }
                std::vector<KeyPoint> keypoints;
                detector->detect(srcImg, keypoints);
                int numPoints = keypoints.size();
                cout << keypoints.size();

                if (numPoints < 450) {
                        cout << "No 450 keypoints available" << endl;
                        continue;
                }

                Mat descriptor;
                detector->compute(srcImg, keypoints, descriptor);
                descriptor.row(0);
                if(descriptor.isContinuous()) {
                        for (int l = 0; l < 450; l++) {
                                vector<float> row;
                                row.assign((float*)descriptor.row(l).data, (float*)descriptor.row(l).data + 128);
//                                _vecmap[row] = id;
                                _featureMT.push_back(row);
                        }
                }
                id += 1;
        }
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


        // Calculate mean vector to center the data

//        cout << "Original descriptor: "<< descriptor;
//        cout << "Converted vector : ";
//        for (auto i : featureMT) {
//                cout << i << " ";
//        }
//        cout << endl;

        return 0;
}

unsigned int Extractor::preprocessing() {
        unsigned int *hashlst = new unsigned int[_L];
        unsigned int hash;

        for (int x = 0; x < 10; x++) {
                vector<float> array = _featureMT.at(x);
                unsigned int imgID =  0;
                if (x % 200 == 0)
                        cout << "in for loop image id = " << imgID << " x = " << x << endl;

                // Old way of computing each keypoints separately
                //                newKP.clear();
                //                newKP.push_back(keypoints.at(x));
                //                detector->compute(srcImg, newKP, descriptor);
                //                if (descriptor.isContinuous()) {
                //                        array.clear();
                //                        array.assign((float*)descriptor.data, (float*)descriptor.data + descriptor.total());
                //                }
                // End of Old way


                cout << "Single vector here: ";
                for (auto i : array) {
                        cout << i << ", ";
                }
                cout << endl << " Vector Size: " <<array.size() << endl;

                // Step 3: Hash each 128-dimensional feature here
                for (int m = 0; m < _L; m++) {      // For lsh, compute each table for each feature
                        srpHash *_srp = new srpHash(128, _K, 1);
                        unsigned int *hashes = _srp->getHash(array, 450);
                        hash = 0;

                        for (int n = 0; n < _srp->_numhashes; n++) {
                                // Convert to an interger
                                //                                cout << hashes[n] << " ";
                                hash += hashes[n] * pow(2, (_srp->_numhashes - n - 1));
                        }
                        hashlst[m] = hash;
                        delete(_srp);
                        delete [] hashes;
                }
                for (int i = 0; i < _L; i++)
                        cout << hashlst[i] << " ";
                cout << endl;
                //                cout << "Before inserting " << endl;
                _lsh ->insert(imgID, hashlst);
                //                cout << "lsh insert successful" << endl;
        }
        return 0;
}

unsigned int Extractor::query(const String &filePath, unsigned int top_k) {
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
        if (numPoints < 450) {
                cout << "Query No 450 keypoints available" << endl;
                return -1;
        }

        // Case when enough KeyPoints
        vector<KeyPoint> newKP;
        Mat descriptor;

        //        unsigned int *hashes;
        unsigned int hash;
        vector<float> array;
        for (int x = 0; x < 450; x++) {
//                cout << "in Query for loop x = " << x << endl;
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
                unsigned int *query = new unsigned int[_L];
                for (int m = 0; m < _L; m++) {      // For lsh, compute each table for each feature
                        srpHash *_srp = new srpHash(128, _K, 1);
                        unsigned int *hashes = _srp->getHash(array, 450);
                        hash = 0;
                        //                        cout << "srp address: " << _srp << endl;
                        //                        cout << "hash address: " << hashes << endl;
                        for (int x = 0; x < _srp->_numhashes; x++) {
                                // Convert to an interger
                                hash += hashes[x] * pow(2, (_srp->_numhashes - x - 1));
                        }
                        query[m] = hash;
//                        cout << "Query insert successful" << endl;
                        delete(_srp);
                        delete [] hashes;
                }
                unsigned int *result = new unsigned int[top_k];
                _lsh->top_k(1, top_k, query, result);
                // Step 4: Update the score of the nearest neighbors.
                for (int i = 0; i < top_k; ++i) {
                        if (_score.count(result[i]) == 0) {
                                _score[result[i]] = 0;
                        }
                        else {
                                _score[result[i]] ++;
                        }
                }
                delete [] query;
                delete [] result;
        }
        // Sort the score map
        vector<pair<int, int> > freq_arr(_score.begin(), _score.end());
        sort(freq_arr.begin(), freq_arr.end(), comparePair());

        _score.clear();
        if (freq_arr[0].first == -1)
                return freq_arr[1].first;
        cout << "Most match score is: " << freq_arr[0].second << endl;
        return freq_arr[0].first;
}

Extractor::~Extractor() {

}
