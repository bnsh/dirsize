#include <stdio.h>
#include <string>
#include <deque>
#include <string.h>
#include <algorithm>
#include <sys/stat.h>
#include <getopt.h>
#include <dirent.h>
#include <errno.h>

using namespace std;

class sizeinfo {
	public:
		sizeinfo(const string& fn,const struct stat& buf,unsigned long long int sz) {
			_filename = fn;
			_size = sz;
			_statinfo = buf;
		}

		const string& filename() const { return(_filename); }
		unsigned long long int size() const { return(_size); }
		time_t mtime() const { return(_statinfo.st_mtime); }
		time_t atime() const { return(_statinfo.st_atime); }
		time_t ctime() const { return(_statinfo.st_ctime); }

		int operator<(const sizeinfo& si) const {
			return((si._statinfo.st_mtime < _statinfo.st_mtime) || ((si._statinfo.st_mtime == _statinfo.st_mtime) && (_filename < si._filename)));
		}
	private:
		string _filename;
		unsigned long long int _size;
		struct stat _statinfo;
};

void traverse(const char *fn,int restrict,dev_t restricted_to_dev,deque<sizeinfo>& sizes) {
	struct stat buf;
	if ((0 == lstat(fn,&buf)) && ((restrict == 0) || (buf.st_dev == restricted_to_dev))) {
		if (S_ISDIR(buf.st_mode)) {
			DIR *dp = opendir(fn);
			if (dp) {
				struct dirent *de;
				while ((de = readdir(dp)) != NULL) {
					if (strcmp(de->d_name,"..") && strcmp(de->d_name,".")) {
						string base = fn;
						base = base + "/" + de->d_name;
						traverse(base.c_str(),restrict,restricted_to_dev,sizes);
					}
				}
				closedir(dp);
			}
		}
		else if (S_ISREG(buf.st_mode)) {
			sizes.push_back(sizeinfo(fn,buf,buf.st_size));
		}
	}
}

int main(int argc,char *argv[]) {
	int errs = 0;
	int c;
	int size = 0;
	int gigs = 0;
	int restrict = 0;
	int atimes = 0;
	int ctimes = 0;
	int mtimes = 1;

	while ((c = getopt(argc,argv,"sgxacm")) != EOF) {
		if (c == 's') size = 1;
		else if (c == 'g') gigs = 1;
		else if (c == 'x') restrict = 1;
		else if (c == 'a') atimes = 1;
		else if (c == 'c') ctimes = 1;
		else if (c == 'm') mtimes = 1;
		else errs++;
	}

	if (!errs) {
		deque<sizeinfo> sizes;

		for (int i = optind; i < argc; ++i) {
			struct stat buf;
			if (0 == lstat(argv[i],&buf)) {
				traverse(argv[i],restrict,buf.st_dev,sizes);
			}
			else { 
				int e = errno;
				fprintf(stderr,"%d: %s!\n",e,strerror(e));
			}
		}
		sort(sizes.begin(),sizes.end());
		for (deque<sizeinfo>::reverse_iterator i = sizes.rbegin(); i != sizes.rend(); ++i) {
			if (size) printf("%llu	",(*i).size());
			if (gigs) {
				const char *units[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB", NULL };
				double u = (*i).size();
				int uno = 0;

				while ((u >= 1024.0) && (units[uno+1] != NULL)) {
					u = u / 1024.0;
					uno++;
				}
				printf("%0.3f %s	",u,units[uno]);
			}
			if (atimes) {
				char buf[1024];
				time_t atime = (*i).atime();
				strftime(buf,1024,"%Y-%m-%d %H:%M:%S",localtime(&atime));
				printf("%s	",buf);
			}
			if (ctimes) {
				char buf[1024];
				time_t ctime = (*i).ctime();
				strftime(buf,1024,"%Y-%m-%d %H:%M:%S",localtime(&ctime));
				printf("%s	",buf);
			}
			if (mtimes) {
				char buf[1024];
				time_t mtime = (*i).mtime();
				strftime(buf,1024,"%Y-%m-%d %H:%M:%S",localtime(&mtime));
				printf("%s	",buf);
			}
			printf("%s\n",(*i).filename().c_str());
		}
	}

	return(0);
}
