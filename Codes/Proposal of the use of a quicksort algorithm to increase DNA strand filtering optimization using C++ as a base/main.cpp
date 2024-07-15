#include iostream
#include fstream
#include cmath

using namespace std;

 Prints content of DNA array
void printDNAarray (const char dnaArray, int length)
{
  cout  Printing DNA Array of length   length  endl;
  for (int i = 0; i  length; i++)
    cout  dnaArray[i];
  cout  endl;
}

 Converts the DNA string into a DNA array, which can be used for sorting 
char 
processDNAString (string line)
{
  char arrayToSort = new char[line.length ()];
  for (int i = 0; i  line.length (); i++)
    arrayToSort[i] = line[i];
  return arrayToSort;
}

 Partition function for quicksort
template typename T
int partition(T array, int low, int high) {
    T pivot = array[high];
    int i = low - 1;
    for (int j = low; j  high; j++) {
        if (array[j] = pivot) {
            i++;
            swap(array[i], array[j]);
        }
    }
    swap(array[i + 1], array[high]);
    return i + 1;
}

 Recursive implementation of quicksort 
template typename T
void quicksortHelper(T array, int low, int high) {
    if (low  high) {
        int pivotIndex = partition(array, low, high);
        quicksortHelper(array, low, pivotIndex - 1);
        quicksortHelper(array, pivotIndex + 1, high);
    }
}

template typename T
void quicksort(T array, int length) {
    quicksortHelper(array, 0, length - 1);
}
 Main procedure for reading input text file that contains DNA string

int main(int argc, char argv[]) {
    ifstream fin;
    string line;
    stdstring cur_dir(argv[0]);

     DNA file is at the same folder than your executable
    int pos = cur_dir.find_last_of();
    string path = cur_dir.substr(0, pos);
    stdcout  path   cur_dir.substr(0, pos)  stdendl;
    stdcout  Exec file   cur_dir.substr(pos + 1)  stdendl;
    string filename = path + rosalind_dna.txt;
    cout  Opening dataset file   filename  endl;
    fin.open(filename);

    if (!fin) {
        cerr  Unable to open rosalind_dna.txt file...  endl;
        cerr  Review your current working directory!!  endl;
        exit(1);
    }

    while (fin) {
        Read a line from file
        getline(fin, line);
        Print line in console
        cout  line  endl;
        There is a single line in the file, so we stop reading
        break;
    }
 cout  endl;

    Process the DNA string into an array such that we can sort it.
    char dnaArray = processDNAString(line);
    printDNAarray(dnaArray, line.length());
    cout  Sorting DNA Array ...  endl;
    quicksort(dnaArray, line.length());
    printDNAarray(dnaArray, line.length());


     Count the occurrences of each type of element in the DNA array
    int counts[4] = { 0, 0, 0, 0 };
    for (int i = 0; i  line.length(); i++) {
        if (dnaArray[i] == 'A') {
            counts[0]++;
        }
        else if (dnaArray[i] == 'C') {
            counts[1]++;
        }
        else if (dnaArray[i] == 'G') {
            counts[2]++;
        }
        else if (dnaArray[i] == 'T') {
            counts[3]++;
        }
    }
     Print the counts for each type of element
    cout  A  counts[0]   C  counts[1]   G  counts[2]   T 
        counts[3]  endl;
    fin.close();
    delete[] dnaArray;
    return 0;
}
