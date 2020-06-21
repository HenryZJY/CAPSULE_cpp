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
        for (int m = 0; m < _L; m++) {
                srpHash *srp = new srpHash(128, _K, 1);
                _storesrp.push_back(srp);
        }
}


unsigned int Extractor::compute(vector<string> files) {
        // Step 1: Detect the keypoints using SURF Detector
        //        double minHessian = 250;
        //        Ptr<SURF> detector = SURF::create(minHessian);
        unsigned int id = 0;
        float *sumvec = new float[128];
        for (int i = 0; i < 128; ++i) {
                sumvec[i] = 0;
        }
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
//                cout << keypoints.size();

                if (numPoints < 450) {
//                        cout << "No 450 keypoints available" << endl;
                        continue;
                }

                // Step 2: Compute feature
                Mat descriptor;
                detector->compute(srcImg, keypoints, descriptor);
//                cout << "Original descriptor of" << filePath  << " ---- "<< descriptor << endl;
                cout << "Storing id is "<< id << endl;
                if(descriptor.isContinuous()) {
                        for (int l = 0; l < 450; l++) {
                                vector<float> row;
                                row.assign((float*)descriptor.row(l).data, (float*)descriptor.row(l).data + 128);
                                for (int j = 0; j < 128; ++j) {
                                        sumvec[j] += row.at(j);
                                }
                                // Store the id of the image at the end of each vector.
                                row.push_back(id);
                                _featureMT.push_back(row);
                        }
                }
                id += 1;
        }

        cout << "Compute Done with " << id << " images" << endl;
        // Calculate the mean vector
//        cout << "Sum vector is ";
        for (int k = 0; k < 128; ++k) {
//                cout << sumvec[k] << " ";
                _meanvec.push_back(sumvec[k] / _featureMT.size());
        }
        cout << endl;
//        cout << "Feature matrix size is " << _featureMT.size() << endl;


        return 0;
//        cout << numPoints << endl;

        // Draw keypoints
        //        Mat img_keypoints;
        //        drawKeypoints(_srcImg, keypoints, img_keypoints);
        //
        //        //-- Show detected (drawn) keypoints
        //        imshow("SURF Keypoints", img_keypoints);
        //
        //        waitKey();

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

}

unsigned int Extractor::preprocessing() {
        unsigned int *hashlst = new unsigned int[_L];
        unsigned int hash;

        float *testvec = new float[128];
        for (int i = 0; i < 128; ++i) {
                testvec[i] = 0;
        }

        for (int x = 0; x < _featureMT.size(); x++) {
                vector<float> array = _featureMT.at(x);
                // Find and remove the image ID
                unsigned int imgID = array.back();
                array.pop_back();
                array = vecminus(array, _meanvec, 128);
                //TODO: Test data centering
                for (int j = 0; j < 128; ++j) {
                        testvec[j] += array.at(j);
                }

                if (x % 225 == 0)
                        cout << "in for loop image id = " << imgID << " x = " << x << endl;
//                cout << " Current image is " << _namemap[imgID] << endl;

                // Old way of computing each keypoints separately
                //                newKP.clear();
                //                newKP.push_back(keypoints.at(x));
                //                detector->compute(srcImg, newKP, descriptor);
                //                if (descriptor.isContinuous()) {
                //                        array.clear();
                //                        array.assign((float*)descriptor.data, (float*)descriptor.data + descriptor.total());
                //                }
                // End of Old way


//                cout << "Single vector here: ";
//                for (auto i : array) {
//                        cout << i << ", ";
//                }
//                cout << endl << " Vector Size: " <<array.size() << endl;

                // Step 3: Hash each 128-dimensional feature here
                for (int m = 0; m < _L; m++) {      // For lsh, compute each table for each feature
                        srpHash *_srp = _storesrp.at(m);
                        unsigned int *hashes = _srp->getHash(array, 450);
                        hash = 0;

                        for (int n = 0; n < _srp->_numhashes; n++) {
                                // Convert to an interger
//                                cout << hashes[n] << "";
                                hash += hashes[n] * pow(2, (_srp->_numhashes - n - 1));
                        }
                        hashlst[m] = hash;
                        delete [] hashes;
//                        cout << "----";
                }

//                for (int i = 0; i < _L; i++)
//                        cout << hashlst[i] << "----";
//                cout << endl << endl;
//                cout << "Inserting image: " << imgID << endl;
                _lsh ->insert(imgID, hashlst);
                //                cout << "lsh insert successful" << endl;
        }
        cout << "Data centered?" << endl;
        for (int k = 0; k < 128; ++k) {
                cout << testvec[k] << "  ";
//                cout << testvec[k] + _meanvec.at(k) * _featureMT.size() << "  ";
        }
        cout << endl;
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
        vector<vector<float>> queryvec;
        Mat descriptor;
        detector->compute(srcImg, keypoints, descriptor);
//        cout << "Original descriptor of" << filePath  << " ---- "<< descriptor << endl;

        if (descriptor.isContinuous()) {
                for (int k = 0; k < 450; ++k) {
                        vector<float> vecrow;
                        vecrow.assign((float*)descriptor.row(k).data, (float*)descriptor.row(k).data + 128);
                        queryvec.push_back(vecrow);
                }
        }

        unsigned int hash;
        for (int x = 0; x < 450; x++) {
//                cout << "in Query for loop x = " << x << endl;

                vector<float> array = vecminus(queryvec.at(x), _meanvec, 128);
                //                for (auto i : array) {
                //                        cout << i << ", ";
                //                }
                //                cout << "\nArray size:" << array.size() << endl;

                // Step 3: Hash each 128-dimensional feature here
                unsigned int *query = new unsigned int[_L];
                for (int m = 0; m < _L; m++) {      // For lsh, compute each table for each feature
//                        cout << "before getting srp from vector" << endl;
                        srpHash *srp = _storesrp.at(m);
//                        cout << "after getting srp from vector" << endl;
                        unsigned int *hashes = srp->getHash(array, 450);
                        hash = 0;
                        //                        cout << "srp address: " << _srp << endl;
                        //                        cout << "hash address: " << hashes << endl;
                        for (int n = 0; n < srp->_numhashes; n++) {
                                // Convert to an interger
//                                cout << hashes[n] << "";
                                hash += hashes[n] * pow(2, (srp->_numhashes - n - 1));
                        }
                        query[m] = hash;
//                        cout << "----";
//                        cout << "Query insert successful" << endl;
                        delete [] hashes;
                }
//                cout << endl << endl;
//                cout << "Before retrieve" << endl;

                unsigned int *result = new unsigned int[_L * RESERVOIR_SIZE];
                _lsh->retrieve(1, query, result);
//                _lsh->top_k(1, top_k, query, result);

                // Step 4: Update the score of the nearest neighbors.

                for (int i = 0; i < _L * RESERVOIR_SIZE; ++i) {
//                        cout << result[i] << "  ";
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
        vector<pair<unsigned int, unsigned int> > freq_arr(_score.begin(), _score.end());
        sort(freq_arr.begin(), freq_arr.end(), comparePair());

        _score.clear();
        if (freq_arr[0].first == -1) {
                cout << endl << "Most match score is: " << freq_arr[1].second << endl;
                return freq_arr[1].first;
        }
        cout << endl << "Most match score is: " << freq_arr[0].second << endl;
        return freq_arr[0].first;
}

Extractor::~Extractor() {

}
