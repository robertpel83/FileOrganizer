#include "OpenFileOrganizer.h"

void go();

OpenFileOrganizer::OpenFileOrganizer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //QDebugStream qout(std::wcout, ui.scanDirsPlainTextEdit);
   // QDebugStream qout(std::wcout, ui.consoleOutputPlainTextEdit);

    //std::wcout << L"Hello this is a test" << std::endl;
    //std::wcout << L"Hello this is a test" << std::endl;
    //std::wcout << L"Hello this is a test" << std::endl;
    //std::wcout << L"Hello this is a test" << std::endl;
    //std::wcout << L"Hello this is a test" << std::endl;


        // Set up ThreadLogStream, which redirect cout to signal sendLogString
     // Set up  MessageHandler,  wgucg catch message from sendLogString and Display

    m_qd = new ThreadLogStream(std::wcout); //Redirect Console output to QTextEdit
    this->msgHandler = new MessageHandler(ui.consoleOutputPlainTextEdit, this);
    connect(m_qd, &ThreadLogStream::sendLogString, msgHandler, &MessageHandler::catchMessage);



    // Connect button signal to appropriate slot
    connect(ui.startPushButton, &QPushButton::released, this, &OpenFileOrganizer::handleButton);
    connect(ui.startPushButton, &QPushButton::clicked, this, &OpenFileOrganizer::handleButton);

}

OpenFileOrganizer::~OpenFileOrganizer()
{}

void OpenFileOrganizer::QMessageOutput(QtMsgType, const QMessageLogContext&, const QString& msg)
{
    std::wcout << msg.toStdWString() << std::endl;
    

}


void Worker::process()
{
    go();
    emit finished();
}



void OpenFileOrganizer::handleButton()
{

    QThread* thread = new QThread;
    Worker* worker = new Worker();
    worker->moveToThread(thread);
    //connect( worker, &Worker::error, this, &OpenFileOrganizer::errorString);
    connect( thread, &QThread::started, worker, &Worker::process);
    connect( worker, &Worker::finished, thread, &QThread::quit);
    connect( worker, &Worker::finished, worker, &Worker::deleteLater);
    connect( thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();

}



















/*
std::wcout << L"recursiveDirectoryIteratorIncrementPaths.size() " << recursiveDirectoryIteratorIncrementPaths.size() << std::endl;
std::wcout << L"directoryIteratorRecursivePaths.size() " << directoryIteratorRecursivePaths.size() << std::endl;

vector<wstring> notFoundMatch1;

for (std::unordered_set<wstring>::const_iterator it = recursiveDirectoryIteratorIncrementPaths.cbegin(); it != recursiveDirectoryIteratorIncrementPaths.cend(); ++it)
{
    wstring path = *it;

    std::unordered_set<wstring>::const_iterator got = directoryIteratorRecursivePaths.find(path);

    if (got == directoryIteratorRecursivePaths.end())
        notFoundMatch1.push_back(path);
}

vector<wstring> notFoundMatch2;

for (std::unordered_set<wstring>::const_iterator it = directoryIteratorRecursivePaths.cbegin(); it != directoryIteratorRecursivePaths.cend(); ++it)
{
    wstring path = *it;

    std::unordered_set<wstring>::const_iterator got = recursiveDirectoryIteratorIncrementPaths.find(path);

    if (got == recursiveDirectoryIteratorIncrementPaths.end())
        notFoundMatch2.push_back(path);
}

std::wcout << L"recursiveDirectoryIteratorIncrementPaths not found in directoryIteratorRecursivePaths " << notFoundMatch1.size() << std::endl;
std::wcout << L"directoryIteratorRecursivePaths not found in recursiveDirectoryIteratorIncrementPaths " << notFoundMatch2.size() << std::endl;

std::wcout << L"recursiveDirectoryIteratorIncrementPaths not found in directoryIteratorRecursivePaths" << std::endl;
for (int i = 0; i < notFoundMatch1.size(); i++)
{
    wstring path = notFoundMatch1[i];
    std::wcout << path << std::endl;
}
std::wcout << L"directoryIteratorRecursivePaths not found in recursiveDirectoryIteratorIncrementPaths" << std::endl;
for (int i = 0; i < notFoundMatch2.size(); i++)
{
    wstring path = notFoundMatch2[i];
    std::wcout << path << std::endl;
}
*/
//findFirstFilePaths


//if (db)
//{
//    fclose(db);
//    db = NULL;
//}







/*
DupeKill_v1.2 ReadMe
Incredibly, I found room on the internet for yet another duplicate file remover.
Like others, this tool compares files based on content and lists any files which have exact binary duplicates.
What sets this one apart is that you can give it hints about which files you'd like to keep before starting a scan.
For example; if you tell it to prefer files with the 'most descriptive name', the app look at all duplicate filenames and default to the shortest,
most descriptive name as the one to keep. A file with a name containing "copy of", or ".1.txt" would be considered less descriptive than one without;
and a file named "lkePic.jpg" is considered less descriptive than "Lake Pictures.jpg". This way the default keep or remove action is pre-set;
and time you spend going though the list and making changes to keep the 'right' file is minimized. The tool has many other options to choose
from to pick the default 'kept' file: use the newest or oldest by date, the longest or shortest filename, or specify one or more
directories where files should always be kept or removed.

The app also makes use of a speed improvement I haven't seen anywhere else:
It makes an extra pass of the file list to create a 'fasthash'. The fasthash uses small samples of the file (16 kilobytes),
taken from the beginning, end, and three places in the middle; then does a duplicate check based on the hash of the samples.
This is very quick for large files, and it helps eliminate the vast majority of potential duplicates very quickly, as
most files will have different samples. Most other duplicate finders omit this step, but it really speeds things up.

As of beta 6 the app is fairly robust; avoiding most of the traps in this excellent reference.

Note: There's no fuzzy searching, this app only finds exact duplicates.

Usage Notes
Version 4.6.2 or better of the .net framework is required. Get it from Microsoft.

No installation; just unpack and run. A settings file and ancillary files may be created in the program folder.

Instructions:

Pick a directory where you have duplicate files and hit scan.
After the program identifies the duplicate files, you'll be given a list to choose what to keep and what to delete.
The app makes suggestions, but you can make changes by right clicking and selecting delete or keep;
Or by clicking the 'Action' column to toggle it. At this point scan results can be copied via the right click menu or
exported in bulk via the 'Export Scan...' button.
Hit the 'Execute Actions' button to run the selected actions. The app will warn you about deletions,
and there's an extra warning if you're deleting all copies of a file.
You can make a log to see what was done. The app will reset after this.
Each duplicate will have an action automatically assigned, and you can manually set the action by
right clicking or using the keyboard. Available actions are: Keep [spacebar], Delete [delete key],
Link [F4 key], Recycle, or Move. The context menu option 'Mark as auto' will reset the action to whatever was
automatically assigned. The 'Invert Action' option will toggle the action between 'Keep' and whatever
the default removal action is (set this in settings, by default it's 'Delete').

For complex searches, the 'Look In' drop-down has an 'Advanced criteria' option.
Advanced criterias let you include or exclude multiple folders and have filtering options,
as well as options to specify a default 'keep' or 'remove from' folder. These criteria can
be named and permanently saved. To create a criteria, select 'Advanced Criteria..." from
the 'Look In' drop-down; the Edit Criteria form will the show with further options.

When using Advanced Criteria the '...' button is used to edit an existing criteria.
To remove a saved criteria: select it in the drop-down, edit it with the '...' button,
and hit the 'Delete this Criteria' button in the lower left.
*/




/*
How does SimilarImages find those similar images?
It uses a somewhat modified version of the "Quadrant RGB" algorithm and futhermore considers some other meta-information provided by the source files.
This basicially means: the images will be split into quadrants and average RGB values are computed.
Pairs that have pretty "close" avg. numbers are considered being "similar".
*/

/*
imagededup is a python package that simplifies the task of finding exact and near duplicates in an image collection.


This package provides functionality to make use of hashing algorithms that are particularly good at finding exact duplicates as well as convolutional neural networks which are also adept at finding near duplicates. An evaluation framework is also provided to judge the quality of deduplication for a given dataset.

Following details the functionality provided by the package:

Finding duplicates in a directory using one of the following algorithms:
Convolutional Neural Network (CNN)
Perceptual hashing (PHash)
Difference hashing (DHash)
Wavelet hashing (WHash)
Average hashing (AHash)
Generation of encodings for images using one of the above stated algorithms.
*/

/*
MobileNets: Efficient Convolutional Neural Networks for Mobile Vision Applications
Andrew G. Howard, Menglong Zhu, Bo Chen, Dmitry Kalenichenko, Weijun Wang, Tobias Weyand, Marco Andreetto, Hartwig Adam
We present a class of efficient models called MobileNets for mobile and embedded vision applications. MobileNets are based on a streamlined architecture that uses depth-wise separable convolutions to build light weight deep neural networks. We introduce two simple global hyper-parameters that efficiently trade off between latency and accuracy. These hyper-parameters allow the model builder to choose the right sized model for their application based on the constraints of the problem. We present extensive experiments on resource and accuracy tradeoffs and show strong performance compared to other popular models on ImageNet classification. We then demonstrate the effectiveness of MobileNets across a wide range of applications and use cases including object detection, finegrain classification, face attributes and large scale geo-localization.
Subjects:	Computer Vision and Pattern Recognition (cs.CV)
Cite as:	arXiv:1704.04861 [cs.CV]
    (or arXiv:1704.04861v1 [cs.CV] for this version)

https://doi.org/10.48550/arXiv.1704.04861
Focus to learn more

*/

/*

So how do you create a perceptual hash? There are a couple of common algorithms, but none are very complicated. (I'm always surprised that the most common algorithms even work, because they seem too simple!) One of the simplest hashes represents a basic average based on the low frequencies.

With pictures, high frequencies give you detail, while low frequencies show you structure. A large, detailed picture has lots of high frequencies. A very small picture lacks details, so it is all low frequencies. To show how the Average Hash algorithm works, I'll use a picture of actress Alyson Hannigan.


Reduce size. The fastest way to remove high frequencies and detail is to shrink the image. In this case, shrink it to 8x8 so that there are 64 total pixels. Don't bother keeping the aspect ratio, just crush it down to fit an 8x8 square. This way, the hash will match any variation of the image, regardless of scale or aspect ratio.

Reduce color. The tiny 8x8 picture is converted to a grayscale. This changes the hash from 64 pixels (64 red, 64 green, and 64 blue) to 64 total colors.
Average the colors. Compute the mean value of the 64 colors.
Compute the bits. This is the fun part. Each bit is simply set based on whether the color value is above or below the mean.
Construct the hash. Set the 64 bits into a 64-bit integer. The order does not matter, just as long as you are consistent. (I set the bits from left to right, top to bottom using big-endian.)
 =  = 8f373714acfcf4d0
The resulting hash won't change if the image is scaled or the aspect ratio changes. Increasing or decreasing the brightness or contrast, or even altering the colors won't dramatically change the hash value. And best of all: this is FAST!

If you want to compare two images, construct the hash from each image and count the number of bit positions that are different. (This is a Hamming distance.) A distance of zero indicates that it is likely a very similar picture (or a variation of the same picture). A distance of 5 means a few things may be different, but they are probably still close enough to be similar. But a distance of 10 or more? That's probably a very different picture.

Getting Funky With pHash
While the Average Hash is quick and easy, it may be too rigid of a comparison. For example, it can generate false-misses if there is a gamma correction or a color histogram is applied to the image. This is because the colors move along a non-linear scale -- changing where the "average" is located and therefore changing which bits are above/below the average.

A more robust algorithm is used by pHash. (I use my own variation of the algorithm, but it's the same concept.) The pHash approach extends the average approach to the extreme, using a discrete cosine transform (DCT) to reduce the frequencies.
Reduce size. Like Average Hash, pHash starts with a small image. However, the image is larger than 8x8; 32x32 is a good size. This is really done to simplify the DCT computation and not because it is needed to reduce the high frequencies.

Reduce color. The image is reduced to a grayscale just to further simplify the number of computations.
Compute the DCT. The DCT separates the image into a collection of frequencies and scalars. While JPEG uses an 8x8 DCT, this algorithm uses a 32x32 DCT.
Reduce the DCT. While the DCT is 32x32, just keep the top-left 8x8. Those represent the lowest frequencies in the picture.
Compute the average value. Like the Average Hash, compute the mean DCT value (using only the 8x8 DCT low-frequency values and excluding the first term since the DC coefficient can be significantly different from the other values and will throw off the average). Thanks to David Starkweather for the added information about pHash. He wrote: "the dct hash is based on the low 2D DCT coefficients starting at the second from lowest, leaving out the first DC term. This excludes completely flat image information (i.e. solid colors) from being included in the hash description."
Further reduce the DCT. This is the magic step. Set the 64 hash bits to 0 or 1 depending on whether each of the 64 DCT values is above or below the average value. The result doesn't tell us the actual low frequencies; it just tells us the very-rough relative scale of the frequencies to the mean. The result will not vary as long as the overall structure of the image remains the same; this can survive gamma and color histogram adjustments without a problem.
Construct the hash. Set the 64 bits into a 64-bit integer. The order does not matter, just as long as you are consistent. To see what this fingerprint looks like, simply set the values (this uses +255 and -255 based on whether the bits are 1 or 0) and convert from the 32x32 DCT (with zeros for the high frequencies) back into the 32x32 image:
 = 8a0303f6df3ec8cd
At first glance, this might look like some random blobs... but look closer. There is a dark ring around her head and the dark horizontal line in the background (right side of the picture) appears as a dark spot.
As with the Average Hash, pHash values can be compared using the same Hamming distance algorithm. (Just compare each bit position and count the number of differences.)

Best in Class?
Since I do a lot of work with digital photo forensics and huge picture collections, I need a way to search for similar pictures. So, I created a picture search tool that uses a couple of different perceptual hash algorithms. In my unscientific but long-term-use experience, I have found that Average Hash is significantly faster than pHash. Average Hash is a great algorithm if you are looking for something specific. For example, if I have a small thumbnail of an image and I know that the big one exists somewhere in my collection, then Average Hash will find it very quickly. However, if there are modifications -- like text was added or a head was spliced into place, then Average Hash probably won't do the job. While pHash is slower, it is very tolerant of minor modifications (minor being less than 25% of the picture).

Then again, if you are running a service like TinEye, then you're not going to compute the pHash every time. I am certain that they have a database of pre-computed hash values. The basic comparison system is extremely fast. (There are some heavily optimized ways to compute a Hamming distance.) So computing the hash is a one-time cost and doing a million comparisons in a few seconds (on one computer) is very realistic.

Variations
There are variations to the perceptual hash algorithm that can also improve performance. For example, the image can be cropped before being reduced in size. This way, extra empty space around the main part of the image won't make a difference. Also, the image can be segmented. For example, if you have a face detection algorithm, then you can compute hashes for each face. (I suspect that TinEye's algorithm does something similar.)

Other variations can track general coloring (e.g., her hair is more red than blue or green, and the background is closer to white than black) or the relative location of lines.

When you can compare images, then you can start doing really cool things. For example, the search engine GazoPa [now offline] allows you to draw a picture. As with TinEye, I don't know the details about how GazoPa works. However, it appears to use a variation of the perceptual hash. Since the hash reduces everything down to the lowest frequencies, my crappy line drawing of three stick figures can be compared with other pictures -- likely matching photos that contain three people.

About 8 months ago I wrote a blog entry on algorithms for comparing pictures. Basically, if you have a large database of pictures and want to find similar images, then you need an algorithm that generates a weighted comparison. In that blog entry, I described how two of the algorithms work:
aHash (also called Average Hash or Mean Hash). This approach crushes the image into a grayscale 8x8 image and sets the 64 bits in the hash based on whether the pixel's value is greater than the average color for the image.

pHash (also called "Perceptive Hash"). This algorithm is similar to aHash but use a discrete cosine transform (DCT) and compares based on frequencies rather than color values.
As a comment to the blog entry, David Oftedal suggested a third option that he called a "difference hash". It took me 6 months to get back to evaluating hash functions and dHash is a definite winner.

dHash
Like aHash and pHash, dHash is pretty simple to implement and is far more accurate than it has any right to be. As an implementation, dHash is nearly identical to aHash but it performs much better. While aHash focuses on average values and pHash evaluates frequency patterns, dHash tracks gradients. Here's how the algorithm works, using the same Alyson Hannigan image as last time:

Reduce size. The fastest way to remove high frequencies and detail is to shrink the image. In this case, shrink it to 9x8 so that there are 72 total pixels. (I'll get to the "why" for the odd 9x8 size in a moment.) By ignoring the size and aspect ratio, this hash will match any similar picture regardless of how it is stretched.
Reduce color. Convert the image to a grayscale picture. This changes the hash from 72 pixels to a total of 72 colors. (For optimal performance, either reduce color before scaling or perform the scaling and color reduction at the same time.)
Compute the difference. The dHash algorithm works on the difference between adjacent pixels. This identifies the relative gradient direction. In this case, the 9 pixels per row yields 8 differences between adjacent pixels. Eight rows of eight differences becomes 64 bits.
Assign bits. Each bit is simply set based on whether the left pixel is brighter than the right pixel. The order does not matter, just as long as you are consistent. (I use a "1" to indicate that P[x] < P[x+1] and set the bits from left to right, top to bottom using big-endian.)
 =  = 3a6c6565498da525
As with aHash, the resulting hash won't change if the image is scaled or the aspect ratio changes. Increasing or decreasing the brightness or contrast, or even altering the colors won't dramatically change the hash value. Even complex adjustments like gamma corrections and color profiles won't impact the result. And best of all: this is FAST! Seriously -- the slowest part of the algorithm is the size reduction step.

The hash values represent the relative change in brightness intensity. To compare two hashes, just count the number of bits that are different. (This is the Hamming distance.) A value of 0 indicates the same hash and likely a similar picture. A value greater than 10 is likely a different image, and a value between 1 and 10 is potentially a variation.

Speed and Accuracy
From FotoForensics, we now have a testbed of over 150,000 images. I have a couple of test images that occur a known number of times. For example, one picture (needle) appears exactly once in the 150,000 image repository (haystack). Another picture occurs twice. A third test picture currently occurs 32 times.

I've used aHash, pHash, and dHash to search for the various needles in the haystack. For comparisons, I did not pre-cache any of the repository hash values. I also consider a cutoff value of 10 to denote a match or a miss. (If the haystack image differs from the needle image by more than 10 bits, then it is assumed to not match.) Here's the results so far:

No hash. This is a baseline for comparison. It loads each image into memory, and then unloads the image. This tells me how much time is spent just on the file access and loading. (And all images are located on an NFS-mounted directory -- so this includes network transfer times.) The total time is 16 minutes. Without any image comparisons, there is a minimum of 16 minutes needed just to load each image.

No hash, Scale. Every one of these hash algorithms begins by scaling the image smaller. Small pictures scale very quickly, but large pictures can take 10 seconds or more. Just loading and scaling the 150,000 images takes 3.75 hours. (I really need to look into possible methods for optimizing my bilinear scaling algorithm.)

aHash. This algorithm takes about 3.75 hours to run. In other words, it takes more time to load and scale the image than to run the algorithm. Unfortunately, aHash generates a huge number of false positives. It matched all of the expected images, but also matched about 10x more false-positives. For example, the test picture that should have matched 32 times actually matched over 400 images. Worse: some of the misses had a difference of less than 2 bits. In general, aHash is fast but not very accurate.

pHash. This algorithm definitely performed the best with regards to accuracy. No false positives, no false negatives, and every match has a score of 2 or less. I'm sure that a bigger data set (or alternate needle image) will generate false matches, but the number of misses will likely be substantially less than aHash.

The problem with pHash is the performance. It took over 7 hours to complete. This is because the DCT computation uses lots of operations including cosine and sine. If I pre-compute the DCT constants, then this will drop 1-2 hours from the overall runtime. But applying the DCT coefficients still takes time. pHash is accurate, but not very fast.

dHash. Absolutely amazing... Very few false positives. For example, the image with two known matches ended up matching 6 pictures total (4 false positives). The scores were: 10, 0, 8, 10, 0, and 10. The two zeros were the correct matches; all of the false-positive matches had higher scores. As speed goes, dHash is as fast as aHash. Well, technically it is faster since it doesn't need to compute the mean color value. The dHash algorithm has all the speed of aHash with very few false-positives.
Algorithm Variations
I have tried a few variations of the dHash algorithm. For example, David's initial proposal used an 8x8 image and wrapped the last comparison (computing the pixel difference between P[0] and P[7] for the 8th comparison). This actually performs a little worse than my 9x8 variation (more false-positives), but only by a little.

Storing values by row or column really doesn't make a difference. Computing both row and column hashes significantly reduces the number of false-positives and is comparable to pHash (almost as accurate). So it maintains speed and gains accuracy as the cost of requiring 128 bits for the hash.

I've also combined pHash with dHash. Basically, I use the really fast dHash as a fast filter. If dHash matches, then I compute the more expensive pHash value. This gives me all the speed of dHash with all the accuracy of pHash.

Finally, I realized that using dHash as a fast filter is good, but I don't need 64-bits for this computation. My 16-bit dHash variant uses a 6x4 reduced image. This gives me 20 difference values. Ignoring the four corners yields a 16-bit hash -- and has the benefit of ignoring the impact from Instagram-like vignetting (corner darkening). If I have a million different images, then I should expect about 15 images per 16-bit dHash. pHash can compare 15 images really quickly. At a billion images, I'm looking at about 15,258 image collisions and that still is a relatively small number.

I can even permit my 16-bit dHash to be sloppy; permitting any 1-bit change to match. Any computed 16-bit dHash would yield 17 possible dHash values to match. A million images should yield about 260 collisions, and a billion becomes about 260,000 collisions. At a billion images, it would be worth storing the 16-bit dHash, 64-bit dHash, and 64-bit pHash values. But a million images? I'm still ahead just by pre-computing the 16-bit dHash and 64-bit pHash values.

Applied Comparisons
There are two things we want out of a perceptual hash algorithm: speed and accuracy. By combining dHash with pHash, we get both. But even without pHash, dHash is still a significant improvement over aHash and without any notable speed difference.
*/



//I've asked this question 5 years ago. This thread was very helpful for me back then, I came to a conclusion, 
//then I moved on with my project. It is funny that I needed something similar recently, totally unrelated to that project from the past. 
//As I was researching for possible solutions, I stumbled upon my own question :)

//The solution I chose now is based on C++11. 
//The boost libraries that Constantin mentions in his answer are now part of the standard.
//If we replace std::wstring with the new string type std::u16string, then the conversions will look like this:
/*
void convertUTF8ToUTF16CodeCvt()
{
    //UTF - 8 to UTF - 16

    std::string source;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string dest = convert.from_bytes(source);

}

void convertUTF16ToUTF8CodeCvt()
{
    //UTF - 16 to UTF - 8

    std::u16string source;

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string dest = convert.to_bytes(source);
}
*/



/*
int main()
{
    const WCHAR* wText = L"Wide string";
    const char* text = convert_from_wstring(wText);
    std::cout << text << "\n";
    free(text);

    const WCHAR *wtext = convert_to_wstring("Multibyte string");
    std::wcout << wtext << "\n";
    free(wtext);

    return 0;
}
*/


/*
//If you are using C++11 or later, have a look at the std::wstring_convert class for converting between UTF strings, eg :

std::wstring convert_to_wstring(const std::string & str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
    return conv.from_bytes(str);
}

std::string convert_from_wstring(const std::wstring & wstr)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
    return conv.to_bytes(wstr);
}
//If you need to interact with other code that is based on char* / wchar_t*,
//std::string as a constructor for accepting char* inputand a c_str() method that can be used for char* output,
//and the same goes for std::wstringand wchar_t* .
*/


/*
#include <iconv.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>

using namespace std;

const size_t BUF_SIZE = 1024;

class IConv {

    iconv_t ic_;

public:
    IConv(const char* to, const char* from)
        : ic_(iconv_open(to, from)) {
        if (iconv_t(-1) == ic_)
            throw std::runtime_error("error from iconv_open()");
    }

    ~IConv()
    {
        if (iconv_t(-1) != ic_) iconv_close(ic_);
    }

    bool convert(char* input, char* output, size_t& out_size) {
        size_t inbufsize = strlen(input) + 1;
        return
            (size_t(-1)
                != iconv(ic_, &input, &inbufsize, &output, &out_size))
            && (0U == inbufsize);
    }
};




int main(void)
{
    char str1[BUF_SIZE] = "tägelîch";
    char str2[BUF_SIZE] = "something else";
    IConv ic("UTF16LE", "ISO_8859-1");
    bool ret;
    size_t outsize = BUF_SIZE;

    ret = ic.convert(str1, str2, outsize);
    if (ret == false) {
        cout << "iconv failed: " << errno << endl;
    }
    else {
       cout << "outsize[" << outsize << "]\n";
       cout << "str1[" << str1 << "]\n";
       cout << "str2[" << str2 << "]\n";

       for ( int i = 0 ; i < (BUF_SIZE - outsize) ; ++i )
          if ( str2[i] )
             cout << "str2[" << i << "]=[" << int(str2[i]) << "]("
                << str2[i] << ")\n";
    }

    return ret ? EXIT_SUCCESS : EXIT_FAILURE;
}


6.5.2 A complete iconv example
The example below features a solution for a common problem.
Given that one knows the internal encoding used by the system for wchar_t strings,
one often is in the position to read text from a file and store it in wide character buffers.
One can do this using mbsrtowcs, but then we run into the problems discussed above.

int file2wcs (int fd, const char *charset, wchar_t *outbuf, size_t avail)
{
  char inbuf[BUFSIZ];
  size_t insize = 0;
  char *wrptr = (char *) outbuf;
  int result = 0;
  iconv_t cd;

  cd = iconv_open ("WCHAR_T", charset);
  if (cd == (iconv_t) -1)
    {
     // Something went wrong.
if (errno == EINVAL)
error(0, 0, "conversion from '%s' to wchar_t not available",
    charset);
else
perror("iconv_open");

// Terminate the output string.
*outbuf = L'\0';

return -1;
    }

    while (avail > 0)
    {
        size_t nread;
        size_t nconv;
        char* inptr = inbuf;

        // Read more input.
        nread = read(fd, inbuf + insize, sizeof(inbuf) - insize);
        if (nread == 0)
        {
            // When we come here the file is completely read.
            //   This still could mean there are some unused
            //   characters in the inbuf.  Put them back.
            if (lseek(fd, -insize, SEEK_CUR) == -1)
                result = -1;

            //Now write out the byte sequence to get into the
            //   initial state if this is necessary.
            iconv(cd, NULL, NULL, &wrptr, &avail);

            break;
        }
        insize += nread;

        //Do the conversion.
        nconv = iconv(cd, &inptr, &insize, &wrptr, &avail);
        if (nconv == (size_t)-1)
        {
            // Not everything went right.  It might only be
            //   an unfinished byte sequence at the end of the
             //  buffer.  Or it is a real problem.
            if (errno == EINVAL)
                // This is harmless.  Simply move the unused
                 //  bytes to the beginning of the buffer so that
                 //  they can be used in the next round.
                memmove(inbuf, inptr, insize);
            else
            {
                // It is a real problem.  Maybe we ran out of
                //   space in the output buffer or we have invalid
                //   input.  In any case back the file pointer to
                 //  the position of the last processed byte.
                lseek(fd, -insize, SEEK_CUR);
                result = -1;
                break;
            }
        }
    }

    // Terminate the output string.
    if (avail >= sizeof(wchar_t))
        *((wchar_t*)wrptr) = L'\0';

    if (iconv_close(cd) != 0)
        perror("iconv_close");

    return (wchar_t*)wrptr - outbuf;
}
This example shows the most important aspects of using the iconv functions.It shows how successive
calls to iconv can be used to convert large amounts of text.The user does not have to care about
stateful encodings as the functions take care of everything.

An interesting point is the case where iconv returns an errorand errno is set to EINVAL.
This is not really an error in the transformation.It can happen whenever the input character set
contains byte sequences of more than one byte for some characterand texts are not processed in one piece.
In this case there is a chance that a multibyte sequence is cut.The caller can then simply read the remainder
of the takesand feed the offending bytes together with new character from the input to iconvand continue the work.
The internal state kept in the descriptor is not unspecified after such an event as is the case with the conversion
functions from the ISO C standard.

The example also shows the problem of using wide character strings with iconv.As explained in the description of the
iconv function above, the function always takes a pointer to a char arrayand the available space is measured in bytes.
In the example, the output buffer is a wide character buffer; therefore, we use a local variable wrptr of type char*,
which is used in the iconv calls.

This looks rather innocent but can lead to problems on platforms that have tight restriction on alignment.
Therefore the caller of iconv has to make sure that the pointers passed are suitable for access of characters
from the appropriate character set.Since, in the above case, the input parameter to the function is a wchar_t pointer,
this is the case (unless the user violates alignment when computing the parameter).But in other situations, especially
when writing generic functions where one does not know what type of character set one usesand, therefore, treats text
as a sequence of bytes, it might become tricky.



*/



//__int64 FileSizeStat(const wchar_t* name)
//{
//    __stat64 buff;
//    if (_wstat64(name, &buff) != 0)
//        return -1; // error, could use errno to find out more
//
//    return buff.st_size;
//}



/*
void convertFromChar()
{
    //Example: Convert from char*
    //This example demonstrates how to convert from a char* to the string types listed above.
    //A char* string(also known as a C - style string) uses a terminating null to indicate the end of the string.
    //C - style strings usually require 1 byte per character, but can also use 2 bytes.
    //In the examples below, char* strings are sometimes referred to as multibyte character strings because of the string data
    //that results from converting from wide Unicode strings.Single byteand multibyte character(MBCS) functions can operate on char* strings.

    using namespace std;


    // Create and display a C-style string, and then use it
    // to create different kinds of strings.
    const char* orig = "Hello, World!";
    cout << orig << " (char *)" << endl;

    // newsize describes the length of the
    // wchar_t string called wcstring in terms of the number
    // of wide characters, not the number of bytes.
    size_t newsize = strlen(orig) + 1;

    // The following creates a buffer large enough to contain
    // the exact number of characters in the original string
    // in the new format. If you want to add more characters
    // to the end of the string, increase the value of newsize
    // to increase the size of the buffer.
    wchar_t* wcstring = new wchar_t[newsize];

    // Convert char* string to a wchar_t* string.
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wcstring, newsize, orig, _TRUNCATE);
    // Display the result and indicate the type of string that it is.
    wcout << wcstring << L" (wchar_t *)" << endl;
    delete[]wcstring;

    // Convert the C-style string to a _bstr_t string.
    _bstr_t bstrt(orig);
    // Append the type of string to the new string
    // and then display the result.
    bstrt += " (_bstr_t)";
    wcout << bstrt << endl;

    // Convert the C-style string to a CComBSTR string.
    CComBSTR ccombstr(orig);
    if (ccombstr.Append(L" (CComBSTR)") == S_OK)
    {
        CW2A printstr(ccombstr);
        cout << printstr << endl;
    }

    // Convert the C-style string to a CStringA and display it.
    CStringA cstringa(orig);
    cstringa += " (CStringA)";
    cout << cstringa << endl;

    // Convert the C-style string to a CStringW and display it.
    CStringW cstring(orig);
    cstring += " (CStringW)";
    // To display a CStringW correctly, use wcout and cast cstring
    // to (LPCTSTR).
    wcout << (LPCTSTR)cstring << endl;

    // Convert the C-style string to a basic_string and display it.
    string basicstring(orig);
    basicstring += " (basic_string)";
    cout << basicstring << endl;

    // Convert the C-style string to a System::String and display it.
    //System::String^ systemstring = gcnew String(orig);
    //systemstring += " (System::String)";
    //Console::WriteLine("{0}", systemstring);
    //delete systemstring;
}
*/


/*
void convertFromWChar()
{

    //Example: Convert from wchar_t*
    //This example demonstrates how to convert from a wchar_t* to other string types.
    //Several string types, including wchar_t*, implement wide character formats.
    //To convert a string between a multibyteand a wide character format, you can use a single function call like mbstowcs_s or a
    //constructor invocation for a class like CStringA.

    using namespace std;

    // Create a string of wide characters, display it, and then
    // use this string to create other types of strings.
    const wchar_t* orig = L"Hello, World!";
    wcout << orig << L" (wchar_t *)" << endl;

    // Convert the wchar_t string to a char* string. Record
    // the length of the original string and add 1 to it to
    // account for the terminating null character.
    size_t origsize = wcslen(orig) + 1;
    size_t convertedChars = 0;

    // Use a multibyte string to append the type of string
    // to the new string before displaying the result.
    char strConcat[] = " (char *)";
    size_t strConcatsize = (strlen(strConcat) + 1) * 2;

    // Allocate two bytes in the multibyte output string for every wide
    // character in the input string (including a wide character
    // null). Because a multibyte character can be one or two bytes,
    // you should allot two bytes for each character. Having extra
    // space for the new string isn't an error, but having
    // insufficient space is a potential security problem.
    const size_t newsize = origsize * 2;
    // The new string will contain a converted copy of the original
    // string plus the type of string appended to it.
    char* nstring = new char[newsize + strConcatsize];

    // Put a copy of the converted string into nstring
    wcstombs_s(&convertedChars, nstring, newsize, orig, _TRUNCATE);
    // append the type of string to the new string.
    _mbscat_s((unsigned char*)nstring, newsize + strConcatsize, (unsigned char*)strConcat);
    // Display the result.
    cout << nstring << endl;
    delete[]nstring;

    // Convert a wchar_t to a _bstr_t string and display it.
    _bstr_t bstrt(orig);
    bstrt += " (_bstr_t)";
    wcout << bstrt << endl;

    // Convert the wchar_t string to a BSTR wide character string
    // by using the ATL CComBSTR wrapper class for BSTR strings.
    // Then display the result.

    CComBSTR ccombstr(orig);
    if (ccombstr.Append(L" (CComBSTR)") == S_OK)
    {
        // CW2A converts the string in ccombstr to a multibyte
        // string in printstr, used here for display output.
        CW2A printstr(ccombstr);
        cout << printstr << endl;
        // The following line of code is an easier way to
        // display wide character strings:
        wcout << (LPCTSTR)ccombstr << endl;
    }

    // Convert a wide wchar_t string to a multibyte CStringA,
    // append the type of string to it, and display the result.
    CStringA cstringa(orig);
    cstringa += " (CStringA)";
    cout << cstringa << endl;

    // Convert a wide character wchar_t string to a wide
    // character CStringW string and append the type of string to it
    CStringW cstring(orig);
    cstring += " (CStringW)";
    // To display a CStringW correctly, use wcout and cast cstring
    // to (LPCTSTR).
    wcout << (LPCTSTR)cstring << endl;

    // Convert the wide character wchar_t string to a
    // basic_string, append the type of string to it, and
    // display the result.
    wstring basicstring(orig);
    basicstring += L" (basic_string)";
    wcout << basicstring << endl;

    // Convert a wide character wchar_t string to a
    // System::String string, append the type of string to it,
    // and display the result.
    //String^ systemstring = gcnew String(orig);
    //systemstring += " (System::String)";
    //Console::WriteLine("{0}", systemstring);
    //delete systemstring;
}
*/

/*
void convertFromBasicString()
{
    //Example: Convert from basic_string
    //This example demonstrates how to convert from a basic_string to other string types.

    using namespace std;

    // Set up a basic_string string.
    string orig("Hello, World!");
    cout << orig << " (basic_string)" << endl;

    // Convert a wide character basic_string string to a multibyte char*
    // string. To be safe, we allocate two bytes for each character
    // in the original string, including the terminating null.
    const size_t newsize = (orig.size() + 1) * 2;

    char* nstring = new char[newsize];
    strcpy_s(nstring, newsize, orig.c_str());
    cout << nstring << " (char *)" << endl;
    delete[]nstring;

    // Convert a basic_string string to a wide character
    // wchar_t* string. You must first convert to a char*
    // for this to work.
    const size_t newsizew = orig.size() + 1;
    size_t convertedChars = 0;
    wchar_t* wcstring = new wchar_t[newsizew];
    mbstowcs_s(&convertedChars, wcstring, newsizew, orig.c_str(), _TRUNCATE);
    wcout << wcstring << L" (wchar_t *)" << endl;
    delete[]wcstring;

    // Convert a basic_string string to a wide character
    // _bstr_t string.
    _bstr_t bstrt(orig.c_str());
    bstrt += L" (_bstr_t)";
    wcout << bstrt << endl;

    // Convert a basic_string string to a wide character
    // CComBSTR string.
    CComBSTR ccombstr(orig.c_str());
    if (ccombstr.Append(L" (CComBSTR)") == S_OK)
    {
        // Make a multibyte version of the CComBSTR string
        // and display the result.
        CW2A printstr(ccombstr);
        cout << printstr << endl;
    }

    // Convert a basic_string string into a multibyte
    // CStringA string.
    CStringA cstring(orig.c_str());
    cstring += " (CStringA)";
    cout << cstring << endl;

    // Convert a basic_string string into a wide
    // character CStringW string.
    CStringW cstringw(orig.c_str());
    cstringw += L" (CStringW)";
    wcout << (LPCTSTR)cstringw << endl;

    // Convert a basic_string string to a System::String
    //String^ systemstring = gcnew String(orig.c_str());
    //systemstring += " (System::String)";
    //Console::WriteLine("{0}", systemstring);
    //delete systemstring;
}
*/


/*
#include <locale>
#include <codecvt>
#include <string>

std::wstring convertNarrowUTF8StringToWideUTF16String(std::string narrow_utf8_source_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(narrow_utf8_source_string);
    return wide;
}

std::string convertWideUTF16StringToNarrowUTF8String(std::wstring wide_utf16_source_string)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::string narrow = converter.to_bytes(wide_utf16_source_string);
    return narrow;

}
*/
