#include <map>
#include <set>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cfloat>
#include <cmath>

#define INVALID_ARGC 1
#define INVALID_CORPUS 2
#define KEY_LEN 26
#define LOWER_BOUND 0.000001

char readChar(FILE *fp) {
	char r;
	do {
		r = tolower(getc(fp));
	} while (!isalpha(r) && r != EOF);

	return r;
}

void getPairDistro(FILE *fp, std::map<std::string, int>& corpus) {
  std::pair<std::map<std::string,int>::iterator,bool> ret;
  char buf[5];
  buf[4] = '\0';
  buf[1] = readChar(fp);
  if (buf[1] == EOF) return;
  buf[2] = readChar(fp);
  if (buf[2] == EOF) return;
  buf[3] = readChar(fp);
  if (buf[3] == EOF) return;


  while (true) {
  	buf[0] = buf[1];
  	buf[1] = buf[2];
  	buf[2] = buf[3];
  	buf[3] = readChar(fp);
  	if (buf[3] == EOF) return;
  	if ((ret = corpus.insert(std::pair<std::string, int>(std::string(buf), 1))).second == false) {
  		(*(ret.first)).second++;
  	}

  }
}

std::string randomKey() {
	std::string key = "abcdefghijklmnopqrstuvwxyz";
	char temp;
	int r;
	for (int i = 0; i < KEY_LEN; i++) {
		temp = key[i];
		r = rand() % KEY_LEN;
		key[i] = key[r];
		key[r] = temp;
	}

	return key;
}

std::string swapTwo(const std::string& key) {
	int r1 = rand() % KEY_LEN;
	int r2;
	char temp;
	std::string r = key;

	do {
		r2 = rand() % KEY_LEN;
	} while(r1 == r2);
	temp = r[r1];
	r[r1] = r[r2];
	r[r2] = temp;

	return r;
}

std::string crypt(const std::string& key, const std::string& msg) {
	std::string result = msg;
	int size = msg.size();
	for (int i = 0; i < size; i++) {
		result[i] = key[msg[i]-'a'];
	}

	return result;
}

double calcSubPart(const std::map<std::string, int>& corpus, const std::string& quadgram) {
	double p = (double)(const_cast<std::map<std::string, int>& >(corpus)[quadgram])/(double)(corpus.size());
	if (p == 0) p = LOWER_BOUND;
	return log(p);
}

double calcScore(const std::map<std::string, int>& corpus, const std::string& decrypted) {
	int size;
	double result;
	if ((size = decrypted.size()) < 4) { 
		fprintf(stderr, "the length of the message is to short\n"); 
		exit(0); 
	}

	for (int i = 0; i <= size-4; i++) {
		double part = calcSubPart(corpus, decrypted.substr(i,4));
		result += part;
	}
	return result;
}

std::string decipher(const std::map<std::string, int>& corpus, const int& trials, const int& swaps, const std::string msg) {
	std::string best_key;
	double best_score = -DBL_MAX;
	std::string decrypted;
	for (int i = 0; i < trials; i++) {
		std::string attempt_key = randomKey();
		double best_trial = -DBL_MAX;
		int since_last_imp = 0;
		std::set<std::string> seen_keys;
		for (int j = 0; j < swaps && since_last_imp < 1000; j++, since_last_imp++) {
			std::string temp_key;
			if (!(seen_keys.insert(temp_key = swapTwo(attempt_key)).second)) {
				j--;
				continue;
			}
			decrypted = crypt(temp_key, msg);
			double score = calcScore(corpus, decrypted);
			if (score > best_trial) {
				since_last_imp = 0;
				best_trial = score;
				attempt_key = temp_key;
			}
		}

		if (best_trial > best_score) {
			best_score = best_trial;
			best_key = attempt_key;
		}
	}

	return best_key;
}

int main(int argc, const char *argv[]) 
{
  FILE *fp;
  int trials;
  int swaps;
  srand(time(NULL));

  // check for the right number of arguments
  if (argc == 3 && strcmp(argv[1], "-e") == 0) {
  	std::string key = randomKey();
  	std::string enc_msg = crypt(key, argv[2]);
  	printf("The message is %s using decrypt key %s\n", enc_msg.c_str(), key.c_str());
  	return 0;
  } else if (argc != 5) {
    fprintf(stderr, "Invalid number of arguments; use ./cipher <corpus> <trials> <swaps> <text>\n");
    return INVALID_ARGC;
  }

  trials = atoi(argv[2]);
  swaps = atoi(argv[3]);

  // check validity of corpus file
  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    fprintf(stderr, "Could not open corpus file \"%s\"\n", argv[1]);
    return INVALID_CORPUS;
  }

  std::map<std::string,int> corpus;
  getPairDistro(fp, corpus);

  for (std::pair<std::string, int> p : corpus) {
  	printf("%s\t%d\n", p.first.c_str(), p.second);
  }

  std::string key = decipher(corpus, trials, swaps, argv[4]);
  printf("The message is %s using decrypt key %s\n", crypt(key, argv[4]).c_str(), key.c_str());

  return 0;
}