#include <iostream>
#include "Extractor.h"
#include <glob.h> // glob(), globfree()
#include <string.h> // memset()
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include "LSH.h"
#include "unordered_map"
using namespace std;

std::vector<string> glob(const std::string& pattern) {
        using namespace std;

        // glob struct resides on the stack
        glob_t glob_result;
        memset(&glob_result, 0, sizeof(glob_result));

        // do the glob operation
        int return_value = glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
        if(return_value != 0) {
                globfree(&glob_result);
                stringstream ss;
                ss << "glob() failed with return_value " << return_value << endl;
                throw std::runtime_error(ss.str());
        }

        // collect all the filenames into a std::list<std::string>
        vector<string> filenames;
        for(size_t i = 0; i < glob_result.gl_pathc; ++i) {
                filenames.push_back(string(glob_result.gl_pathv[i]));
        }

        // cleanup
        globfree(&glob_result);

        // done
        return filenames;
}

int
main() {
        // Preprocessing

        int n = 5;
        LSH *lsh = new LSH();
        vector<string> files = glob("/Users/henryzjy/Desktop/Projects/CAPSULE_cpp/training/*/*.jpg");
//        cout << files.size() << endl;
        Extractor extractor = Extractor(RANGE_POW, lsh, NUM_TABLES);
        unsigned int result = extractor.compute(files);
        extractor.preprocessing();

        lsh->view();

        // Query
        extractor._namemap[-1] = "BOOM";
        vector<string> queries = glob("/Users/henryzjy/Desktop/Projects/CAPSULE_cpp/query/*.jpg");
        for (auto query : queries) {
                unsigned int match;

//                unsigned int *r = new unsigned int[2400]; // 400 Features, find top-6 neighbors for each feature

//                auto top_k = print_N_mostFrequentNumber(r, 2400, n);
//                for (int i = 0; i < n; i++) {
//                        cout << umap[top_k[i]] << endl;
//                }

                cout << "Querying " << query << endl;
                if ((match = extractor.query(query, 20)) != -1) {
                        cout << "Match is " << extractor._namemap[match] << endl;
                        cout << endl;
                }
        }

        return 0;
}
