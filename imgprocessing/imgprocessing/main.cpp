
#include <stdio.h>
#include <iostream>
#include "sumsquaredistance.h"
#include "correlcoeff.h"
#include "surfcomp.h"
#include "keypointcontrol.h"
#include "histcompare.h"
#include <ctime>
using namespace std;

//initial time ~ 22.6s

/*	TODO:
	Rather than storing histogram entry for each image, see if you can store them in a data structure (red black tree?)
	and store that instead (to achieve faster access and potentially lower storage requirement)

	How the fuck do you store red black trees (map)

	Look at effect of changing number of columns for hue and saturation
	 - increases file size and execution time but improves accuracy (I think)

	Read histograms from file rather than iterating through images (avoids errors)
	 - can only be done (that I know of) if theyre saved as a vector of histograms
*/

int main()
{
	clock_t t = clock();

	//keypointcontrol kc;
	//kc.computekeypoints("../memes/*.png");

	//surfcomp sc;
	//int result = sc.genfeatures("../sponge2.png", "../memes/*.png");

	//sumsquaredistance ssd; //lower = similar
	//int result = ssd.calculatessd("../sponge.png", "../memes/*.png");

	//correlcoeff ccf; //higher = similar
	//int result = ccf.calculateccf("../sponge.jpg", "../images/*.jpg");

	histcompare hc;
	hc.computegrams("../memes/*.png");
	//int result = hc.comparegrams("../fish.jpg", "../holiday/*.jpg");

	t = clock() - t;
	cout << "Time elapsed: " << t / double(CLOCKS_PER_SEC) << "s" << endl;



	return 0;
}
