#include <iostream>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>
#include <forward_list>
#include <deque>
#include <queue>
#include <stack>

using namespace std;

// https://levelup.gitconnected.com/8-ways-to-measure-execution-time-in-c-c-48634458d0f9

class SimpleClock {
private:
    unordered_map<string, SimpleClock> namedClocks;  // Can have multiple clocks.
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    std::chrono::time_point<std::chrono::high_resolution_clock> end;
public:
    // The default constructor just creates the class for usage.
    SimpleClock() {}

    // Inserts the clock if it does not already exist.
    bool insertClock(string key) {
        SimpleClock sc;
        pair<string, SimpleClock> p(key, sc);
        auto retval = namedClocks.insert(p);
        return retval.second;
    }

    // Starts the clock if it exists.
    bool start(string key) {
        auto retval = namedClocks.find(key);
        if (retval == namedClocks.end()) return false;
        else {
            retval->second.begin = std::chrono::high_resolution_clock::now();
            return true;
        }
    }

    // Stops the clock if it exists.
    bool stop(string key) {
        auto retval = namedClocks.find(key);
        if (retval == namedClocks.end()) return false;
        else {
            retval->second.end = std::chrono::high_resolution_clock::now();
            return true;
        }
    }

    // Gets the time in (milli)seconds, if the clock exists.
    double getDiff(string key) {
        auto retval = namedClocks.find(key);
        if (retval == namedClocks.end()) return -1;
        else {
            // Just to remember. If you want to do changes to the object referenced by the iterator, take an address of it!!!
            // Otherwise you will get a copy of it.
            //SimpleClock & sc = retval->second;
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(retval->second.end - retval->second.begin);
            return elapsed.count() * 1e-3;
        }
    }
};

struct inner_struct {
    unsigned value;
    inner_struct(unsigned k) : value(k) {};

    inner_struct(const inner_struct& a) : value(a.value) {
        //std::cout << "Copy inner_struct\n"; 
    };
    
    ~inner_struct(){
      //  std::cout << "Destructor inner_struct\n";
    }

    inner_struct& operator=(const inner_struct& a) {
       // std::cout << "Assigment inner_struct\n";
        if (this == &a) return *this;
        value = a.value;
        return *this;
    }
};

struct outer_struct {

    unsigned value;
    inner_struct* inner_value;

    outer_struct(unsigned k, unsigned j) : value(k), inner_value(nullptr) {
        set_inner_value(j);
    };

    void set_inner_value(unsigned j) {
        inner_value = new inner_struct(j);
    }

    outer_struct(const outer_struct& a){
        value = a.value;
        inner_value = new inner_struct(*a.inner_value);
     //   std::cout << "Copy outer_struct\n";
    };

    ~outer_struct() {
        if (inner_value) delete inner_value;
        inner_value = nullptr;
      //  std::cout << "Destructor outer_struct\n";
    }
    
    outer_struct& operator=(const outer_struct& a) {
      //  std::cout << "Assigment outer_struct\n";
        if (this == &a) return *this;
        if (inner_value != NULL) delete inner_value;
        value = a.value;
        inner_value = new inner_struct(a.inner_value->value);
        return *this;
    }
   
 
};

struct outer_struct_different_copy {

    unsigned value;
    inner_struct inner_value;

    outer_struct_different_copy(unsigned k, unsigned j) : value(k), inner_value(NULL) {
        set_inner_value(j);
    };

    void set_inner_value(unsigned j) {
        inner_value = inner_struct(j);
    }

    outer_struct_different_copy(const outer_struct_different_copy& a) : value(a.value), inner_value(inner_struct(a.inner_value)) { 
     //   std::cout << "Copy outer_struct\n"; 
    };

    ~outer_struct_different_copy() {
     //   std::cout << "Destructor outer_struct\n";
    }

    outer_struct_different_copy& operator=(const outer_struct_different_copy& a) {
     //   std::cout << "Assigment outer_struct\n";
        if (this == &a) return *this;
        value = a.value;
        inner_value = inner_struct(a.inner_value);
        return *this;
    }
};

struct outer_struct_no_inner {
    unsigned value;
    outer_struct_no_inner(unsigned k) : value(k) {};
    outer_struct_no_inner(const outer_struct_no_inner& a) {
       // std::cout << "Copy outer_struct_no_inner\n"; 
        value = a.value;
    };
    ~outer_struct_no_inner() {
        //  std::cout << "Destructor outer_struct_no_inner\n";
    }
    outer_struct_no_inner& operator=(const outer_struct_no_inner& a) {
       // std::cout << "Assigment outer_struct_no_inner\n";
        if (this == &a) return *this;
        value = a.value;
        return *this;
    }
};


const int ELEMENTS = 100000; // 1000000 used in performance test run 
const int ELEMENTS_DEL = ELEMENTS / 500;
const int DATA_STRUCTURES = 6;
const int ROUNDS = 10;  // 20 used in performance test
void populate_test() {
    std::cout << "Testing populating with int elements\n";
    vector<int> v;
   // v.reserve(ELEMENTS);
    list<int> l;
    forward_list<int> fl;
    deque<int> d;
    stack<int> s;
    queue<int> q;
    string names[DATA_STRUCTURES] = { "Vector","List","FL","Deque","Stack","Queue" };
    int times[DATA_STRUCTURES] = { 0 };
    vector<vector<double>> diffs(DATA_STRUCTURES);
    srand(time(nullptr));
    SimpleClock sc;
    sc.insertClock("Testi");
    for (int j = 0; j < DATA_STRUCTURES * ROUNDS; j++) {

        // Choosing the data structure to be tested by random.
        //cout << j << endl;
        int ds_to_choose;
        do {
            ds_to_choose = rand() % 6;
        } while (times[ds_to_choose] >= ROUNDS);
        //cout << ds_to_choose << endl;
        times[ds_to_choose]++;

        // See that clearing (emptying) the structure must not be measured.
        if (ds_to_choose == 0) {  // The vector.
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                v.push_back(i);
            }
            sc.stop("Testi");
            v.clear();
          //  vector<int>(v).swap(v);
        }
        

        else if (ds_to_choose == 1) {  // List
            l.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                l.push_back(i);
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 2) {  // Forward list
            fl.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                fl.push_front(i);  // Should reverese to get elements in same order. Does not support push_back.
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 3) {  // Deque
            d.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                d.push_back(i);
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 4) {  // Stack
            while (!s.empty()) s.pop();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                s.push(i);
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 5) {  // Queue
            q = queue<int>();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                q.push(i);
            }
            sc.stop("Testi");
        }
    //    std::cout << "V cap: " << v.capacity() << "\n";
        diffs[ds_to_choose].push_back(sc.getDiff("Testi"));
    }
    for (int i = 0; i < DATA_STRUCTURES; i++)
        printf("%6s", names[i].c_str());
    cout << endl;
    for (int j = 0; j < diffs[0].size(); j++) {
        for (int i = 0; i < DATA_STRUCTURES; i++)
            printf("%6.2f", diffs[i][j]);
        cout << endl;
    }
}

void populate_test_struct_inner() {
    std::cout << "Testing populating with struct elements that have a 'new' pointer to an another struct\n";
    vector<outer_struct> v;
    //v.reserve(ELEMENTS);
    list<outer_struct> l;
    forward_list<outer_struct> fl;
    deque<outer_struct> d;
    stack<outer_struct> s;
    queue<outer_struct> q;
    string names[DATA_STRUCTURES] = { "Vector","List","FL","Deque","Stack","Queue" };
    int times[DATA_STRUCTURES] = { 0 };
    vector<vector<double>> diffs(DATA_STRUCTURES);
    srand(time(nullptr));
    SimpleClock sc;
    sc.insertClock("Testi");
    for (int j = 0; j < DATA_STRUCTURES * ROUNDS; j++) {

        // Choosing the data structure to be tested by random.
        //cout << j << endl;
        int ds_to_choose;
        do {
            ds_to_choose = rand() % 6;
        } while (times[ds_to_choose] >= ROUNDS);
        //cout << ds_to_choose << endl;
        times[ds_to_choose]++;

        // See that clearing (emptying) the structure must not be measured.
        if (ds_to_choose == 0) {  // The vector.

            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                v.push_back(outer_struct(i,i+1));
            }
            sc.stop("Testi");
            v.clear();
        //    vector<outer_struct>(v).swap(v);
        }

        else if (ds_to_choose == 1) {  // List
            l.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                l.push_back(outer_struct(i, i + 1));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 2) {  // Forward list
            fl.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                fl.push_front(outer_struct(i, i + 1));  // Should reverese to get elements in same order. Does not support push_back.
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 3) {  // Deque
            d.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                d.push_back(outer_struct(i, i + 1));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 4) {  // Stack
            while (!s.empty()) s.pop();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                s.push(outer_struct(i, i + 1));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 5) {  // Queue
            q = queue<outer_struct>();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                q.push(outer_struct(i, i + 1));
            }
            sc.stop("Testi");
        }
        diffs[ds_to_choose].push_back(sc.getDiff("Testi"));
    }
    for (int i = 0; i < DATA_STRUCTURES; i++)
        printf("%6s", names[i].c_str());
    cout << endl;
    for (int j = 0; j < diffs[0].size(); j++) {
        for (int i = 0; i < DATA_STRUCTURES; i++)
            printf("%6.2f", diffs[i][j]);
        cout << endl;
    }

}

void populate_test_struct_inner_different_copy() {
    std::cout << "Testing populating with struct elements that have a non-'new' member of another struct\n";
    vector<outer_struct_different_copy> v;
   // v.reserve(ELEMENTS);
    list<outer_struct_different_copy> l;
    forward_list<outer_struct_different_copy> fl;
    deque<outer_struct_different_copy> d;
    stack<outer_struct_different_copy> s;
    queue<outer_struct_different_copy> q;
    string names[DATA_STRUCTURES] = { "Vector","List","FL","Deque","Stack","Queue" };
    int times[DATA_STRUCTURES] = { 0 };
    vector<vector<double>> diffs(DATA_STRUCTURES);
    srand(time(nullptr));
    SimpleClock sc;
    sc.insertClock("Testi");
    for (int j = 0; j < DATA_STRUCTURES * ROUNDS; j++) {

        // Choosing the data structure to be tested by random.
        //cout << j << endl;
        int ds_to_choose;
        do {
            ds_to_choose = rand() % 6;
        } while (times[ds_to_choose] >= ROUNDS);
        //cout << ds_to_choose << endl;
        times[ds_to_choose]++;

        // See that clearing (emptying) the structure must not be measured.
        if (ds_to_choose == 0) {  // The vector.

            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                v.push_back(outer_struct_different_copy(i, i + 1));
            }
            sc.stop("Testi");
            v.clear();
        //    vector<outer_struct_different_copy>(v).swap(v);
        }

        else if (ds_to_choose == 1) {  // List
            l.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                l.push_back(outer_struct_different_copy(i, i + 1));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 2) {  // Forward list
            fl.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                fl.push_front(outer_struct_different_copy(i, i + 1));  // Should reverese to get elements in same order. Does not support push_back.
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 3) {  // Deque
            d.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                d.push_back(outer_struct_different_copy(i, i + 1));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 4) {  // Stack
            while (!s.empty()) s.pop();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                s.push(outer_struct_different_copy(i, i + 1));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 5) {  // Queue
            q = queue<outer_struct_different_copy>();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                q.push(outer_struct_different_copy(i, i + 1));
            }
            sc.stop("Testi");
        }
        diffs[ds_to_choose].push_back(sc.getDiff("Testi"));
    }
    for (int i = 0; i < DATA_STRUCTURES; i++)
        printf("%6s", names[i].c_str());
    cout << endl;
    for (int j = 0; j < diffs[0].size(); j++) {
        for (int i = 0; i < DATA_STRUCTURES; i++)
            printf("%6.2f", diffs[i][j]);
        cout << endl;
    }

}

void populate_test_struct() {
    std::cout << "Testing populating with a struct (no pointer to another)\n";
    vector<outer_struct_no_inner> v;
 //   v.reserve(ELEMENTS);
    list<outer_struct_no_inner> l;
    forward_list<outer_struct_no_inner> fl;
    deque<outer_struct_no_inner> d;
    stack<outer_struct_no_inner> s;
    queue<outer_struct_no_inner> q;
    string names[DATA_STRUCTURES] = { "Vector","List","FL","Deque","Stack","Queue" };
    int times[DATA_STRUCTURES] = { 0 };
    vector<vector<double>> diffs(DATA_STRUCTURES);
    srand(time(nullptr));
    SimpleClock sc;
    sc.insertClock("Testi");
    for (int j = 0; j < DATA_STRUCTURES * ROUNDS; j++) {

        // Choosing the data structure to be tested by random.
        //cout << j << endl;
        int ds_to_choose;
        do {
            ds_to_choose = rand() % 6;
        } while (times[ds_to_choose] >= ROUNDS);
        //cout << ds_to_choose << endl;
        times[ds_to_choose]++;

        // See that clearing (emptying) the structure must not be measured.
        if (ds_to_choose == 0) {  // The vector.
            
           
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                v.push_back(outer_struct_no_inner(i));
            }
            sc.stop("Testi");
            v.clear();
      //      vector<outer_struct_no_inner>(v).swap(v);
        }

        else if (ds_to_choose == 1) {  // List
            l.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                l.push_back(outer_struct_no_inner(i));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 2) {  // Forward list
            fl.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                fl.push_front(outer_struct_no_inner(i));  // Should reverese to get elements in same order. Does not support push_back.
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 3) {  // Deque
            d.clear();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                d.push_back(outer_struct_no_inner(i));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 4) {  // Stack
            while (!s.empty()) s.pop();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                s.push(outer_struct_no_inner(i));
            }
            sc.stop("Testi");
        }

        else if (ds_to_choose == 5) {  // Queue
            q = queue<outer_struct_no_inner>();
            sc.start("Testi");
            for (int i = 0; i < ELEMENTS; i++) {
                q.push(outer_struct_no_inner(i));
            }
            sc.stop("Testi");

        }
        diffs[ds_to_choose].push_back(sc.getDiff("Testi"));
    }
    for (int i = 0; i < DATA_STRUCTURES; i++)
        printf("%6s", names[i].c_str());
    cout << endl;
    for (int j = 0; j < diffs[0].size(); j++) {
        for (int i = 0; i < DATA_STRUCTURES; i++)
            printf("%6.2f", diffs[i][j]);
        cout << endl;
    }
}

void destruct_test() {
    vector<int> v;
    list<int> l;
    SimpleClock sc;
    sc.insertClock("v");
    sc.insertClock("l");
    for (int i = 0; i < ROUNDS; i++) {
        for (int i = 0; i < ELEMENTS_DEL; i++) {
            v.push_back(i);
            l.push_back(i);
        }
        sc.start("v");
        for (int i = 0; i < ELEMENTS_DEL; i++) {
            v.erase(v.begin());
        }
        sc.stop("v");
        sc.start("l");
        for (int i = 0; i < ELEMENTS_DEL; i++) {
            l.erase(l.begin());
        }
        sc.stop("l");
        printf("v: %6.2f seconds.\n", sc.getDiff("v"));
        printf("l: %6.2f seconds.\n", sc.getDiff("l"));
        v.clear();
        l.clear();
    }
}

void vector_list_del_compare() {
    int elements = 1000;
    vector<int> v;
    list<int> l;
    SimpleClock sc;
    sc.insertClock("v");
    sc.insertClock("l");
    cout << "press cntr-c when getting tired :)" << endl;
    while (elements <= ELEMENTS) {
        for (int i = 0; i < elements; i++) {
            v.push_back(i);
            l.push_back(i);
        }
        sc.start("v");
        for (int i = 0; i < elements; i++) {
            v.erase(v.begin());
        }
        sc.stop("v");
        sc.start("l");
        for (int i = 0; i < elements; i++) {
            l.erase(l.begin());
        }
        sc.stop("l");
        printf("v (%10d) %6.2f seconds.\n", elements, sc.getDiff("v"));
        printf("l (%10d) %6.2f seconds.\n\n", elements, sc.getDiff("l"));
        v.clear();
        l.clear();
        if (elements % 10000 == 0)
            cout << "press cntr-c when getting tired :)" << endl;
        elements += 1000;
    }
}

int main() {

    populate_test();
    populate_test_struct();
    populate_test_struct_inner();
    populate_test_struct_inner_different_copy();

    return 0;
}
