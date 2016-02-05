#include<queue>
#include <stdio.h>
using namespace std;

class guber
{
	public:
	void push(int qel) {
		q.push(qel);
	};
	int pop() {
		int tmp;
		tmp = q.front();
		q.pop();
		return(tmp);
	};
	private:
	queue <int> q;
};

main()
{
    guber gub;

    gub.push(3);
    gub.push(5);
    gub.push(7);
    printf("got %d\n",gub.pop());
    printf("got %d\n",gub.pop());
    printf("got %d\n",gub.pop());
}
