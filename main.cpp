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

int mostFrequent(unsigned int arr[], int n) {
        // Sort the array
        sort(arr, arr + n);

        // find the max frequency using linear traversal
        int max_count = 1, res = arr[0], curr_count = 1;
        for (int i = 1; i < n; i++) {
                if (arr[i] == arr[i - 1])
                        curr_count++;
                else {
                        if (curr_count > max_count) {
                                max_count = curr_count;
                                res = arr[i - 1];
                        }
                        curr_count = 1;
                }
        }

        // If last element is most frequent
        if (curr_count > max_count)
        {
                max_count = curr_count;
                res = arr[n - 1];
        }

        return res;
}

//// comparison function to sort the 'freq_arr[]'
//struct comparePair
//{
//        bool operator()(pair<int, int> p1, pair<int, int> p2)
//        {
//                // if frequencies of two elements are same
//                // then the larger number should come first
//                if (p1.second == p2.second)
//                        return p1.first > p2.first;
//
//                // insert elements in the priority queue on the basis of
//                // decreasing order of frequencies
//                return p1.second > p2.second;
//        }
//};

// funnction to print the k numbers with most occurrences
unsigned int *print_N_mostFrequentNumber(unsigned int arr[], int n, int k)
{
        // unordered_map 'um' implemented as frequency hash table
        unordered_map<int, int> um;
        for (int i = 0; i<n; i++)
                um[arr[i]]++;

        auto result_arr = new unsigned int[k];
        // store the elements of 'um' in the vector 'freq_arr'
        vector<pair<int, int> > freq_arr(um.begin(), um.end());

        // sort the vector 'freq_arr' on the basis of the
        // 'compare' function
        sort(freq_arr.begin(), freq_arr.end(), comparePair());

        // display the top k numbers
        cout <<  " numbers of the most occurrences are:\n";
        for (int i = 0; i<k; i++) {
                cout << freq_arr[i].second << " ";
                result_arr[i] = freq_arr[i].first;
        }
         return result_arr;
}

int
main() {
        // Preprocessing

        int n = 5;
        LSH *lsh = new LSH();
        vector<string> files = glob("/Users/henryzjy/Desktop/Projects/CAPSULE_cpp/small/*/*.jpg");
        Extractor extractor = Extractor(RANGE_POW, lsh, NUM_TABLES);
        unordered_map<int, string> umap;        // Mapping img ids with img names.
        unsigned int x = 0;
        for (auto img : files) {
                umap[x] = img;
                int result = extractor.computeAndHash(img, x);
                x += 1;
//                cout << "result" << result << endl;
        }
        lsh->view();

        // Query
        vector<string> queries = glob("/Users/henryzjy/Desktop/Projects/CAPSULE_cpp/query/*.jpg");
        for (auto query : queries) {
                unsigned int match;
//                unsigned int *r = new unsigned int[2400]; // 400 Features, find top-6 neighbors for each feature

//                auto top_k = print_N_mostFrequentNumber(r, 2400, n);
//                for (int i = 0; i < n; i++) {
//                        cout << umap[top_k[i]] << endl;
//                }
                cout << "Querying " << query << endl;
                if ((match = extractor.query(query, 10)) != -1) {
                        cout << "Match is " << umap[match] << endl;
                        cout << endl;
                }
        }

        return 0;
}
