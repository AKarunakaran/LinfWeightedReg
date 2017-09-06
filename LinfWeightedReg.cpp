#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <set>
#include <limits>
using namespace std;

void readInput(vector<double>& data, vector<double>& weights, vector<vector<int>>& dag) {
	int n, m;
	cin >> n >> m;
	data.resize(n);
	weights.resize(n);
	dag.resize(n);
	for(int i = 0; i < n; ++i) {
		cin >> data[i] >> weights[i];
	}
	for(int i = 0; i < m; ++i) {
		int v1, v2;
		cin >> v1 >> v2;
		dag[v1].push_back(v2);
	}
}

void findRandomErrors(vector<double>& data, vector<double>& weights, vector<double>& errors) {
	srand(time(NULL));
	int n = data.size(), numPairs = data.size() * log2(data.size());
	multiset<double> errorSet;
	for(int i = 0; i < numPairs; ++i) {
		int first, second;
		first = rand() % n;
		second = rand() % n;
		while(second == first) 
			second = rand() % n;
		if(data[first] > data[second]) {
			int temp = first;
			first = second;
			second = temp;
		}
		double err;
		if(data[first] == data[second]) {
			err = 0;
		} else {
			err = weights[first]*weights[second]*(data[second]-data[first])/(weights[first]-weights[second]);
		}
		errorSet.insert(err);
	}
	errors.push_back(0);
	for(auto itr = errorSet.begin(); itr != errorSet.end(); ++itr) {
		errors.push_back(*itr);
	}
	errors.push_back(numeric_limits<double>::max());
}

bool feasibilityTest(vector<double>& data, vector<vector<int>>& dag, double eps) {
	vector<int> inDegree(data.size(), 0), reg(data.size(), 0);
	queue<int> next;
	for(int i = 0; i < dag.size(); ++i) {
		for(int v : dag[i]) {
			++inDegree[v];
		}
	}
	for(int i = 0; i < data.size(); ++i) {
		if(inDegree[i] == 0) {
			next.push(i);
			reg[i] = data[i] - eps;
		}
	}
	while(!next.empty()) {
		int cur = next.front();
		next.pop();
		for(int v : dag[cur]) {
			if(data[v] + eps < reg[cur]) {
				return false;
			}
			reg[v] = max(reg[cur], max(data[v]-eps, reg[v]));
			--inDegree[v];
			if(inDegree[v] == 0) {
				next.push(v);
			}
		}
	}
	return true;
}

void binarySearch(vector<double>& data, vector<vector<int>>& dag, vector<double>& errors) {
	int left = 0, right = errors.size();
	while(left < right) {
		int mid = (left+right)/2;
		if(feasibilityTest(data, dag, errors[mid])) {
			right = mid-1;
		} else {
			left = mid+1;
		}
	}
	return left;
}

int main() {
	vector<double> data, weights, errors;
	vector<vector<int>> dag;
	readInput(data, weights, dag);
	findRandomErrors(data, weights, errors);
	int epsIdx = binarySearch(data, dag, errors);

	return 0;
}