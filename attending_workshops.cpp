/*
 * url: https://www.hackerrank.com/challenges/attending-workshops/problem
 * crusty starting code, but I learned about a new algorithm here.
 */

#include<bits/stdc++.h>
//have to do this because the main is not changable in this task.
using namespace std;

/*
 * this task comes with unchangable code that I needed to work with.
 */

struct workshop {
    workshop(int startTime, int duration, int endTime) : start_time(startTime), duration(duration), end_time(endTime) {}
    int start_time;
    int duration;
    int end_time;
};

struct Available_Workshops {
    int n;
    std::vector<workshop*> ws;
};

Available_Workshops* initialize (int start_time[], int duration[], int n){
    auto* worksshops = new Available_Workshops();
    worksshops->n = n;
    worksshops->ws.resize(n);
    for (int i = 0; i < n; i++){
        worksshops->ws[i] = new workshop(start_time[i],duration[i],start_time[i] + duration[i]);
    }
    return worksshops;
};

int CalculateMaxWorkshops(Available_Workshops* ptr){
    int res = 0;
    std::vector<workshop*> localws = ptr->ws;
    std::vector<workshop*> candidates = ptr->ws;
    std::vector<workshop*> selection;
    //this is an intervall scheduling problem
    //lets be greedy
    //sort the workshops by end time
    std::sort(candidates.begin(), candidates.end(), [](workshop* a, workshop* b){
        return (a->end_time < b->end_time);
    });
    //remove all workshops that intersect the one that ends first
    while (!candidates.empty()){
        selection.push_back(candidates.front());
        candidates.erase(std::find(candidates.begin(), candidates.end(), candidates.front()));
        candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [&](workshop* ws){
            return ((ws->start_time < selection.back()->end_time));
        }), candidates.end());
    }

    return selection.size();

};

//Define the structs Workshops and Available_Workshops.
//Implement the functions initialize and CalculateMaxWorkshops

int main(int argc, char *argv[]) {
    int n; // number of workshops
    cin >> n;
    // create arrays of unknown size n
    int* start_time = new int[n];
    int* duration = new int[n];

    for(int i=0; i < n; i++){
        cin >> start_time[i];
    }
    for(int i = 0; i < n; i++){
        cin >> duration[i];
    }

    Available_Workshops * ptr;
    ptr = initialize(start_time,duration, n);
    cout << CalculateMaxWorkshops(ptr) << endl;
    return 0;
}