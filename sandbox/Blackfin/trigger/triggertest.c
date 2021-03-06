#include <stdio.h>
#include "trigger.h"
#include "dataset.h"



int main()
{
//     testDataset();
//     struct dataset * s = loadDataset("/home/steve/r0.bin");
//     blockTrigger(s, 0, 214456);

    struct dataset * s = loadDataset("/home/steve/r90.bin");
    blockTrigger(s, 0, 132704);

//     struct dataset * s = loadDataset("/home/steve/r180.bin");
//     blockTrigger(s, 0, 99368);

//     struct dataset * s = loadDataset("/home/steve/r270.bin");
//     blockTrigger(s, 0, 226472);

//     struct dataset * s = loadDataset("/home/steve/r360.bin");
//     blockTrigger(s, 0, 97320);
}



int testDataset()
{
    struct dataset * s = createDataset(10000);
    if(s == NULL)
    {
        printf("Crap\n");
        return -1;
    }

    signed short cs = 0;

    int i=0, j=0;

    for(i=0; i<10000; i++)
        for(j=0; j<4; j++)
        {
            putSample(s, j, i, cs);
            cs++;
        }
    cs = 0;

    for(i=0; i<10000; i++)
        for(j=0; j<4; j++)
        {
            if(getSample(s, j, i) != cs)
                printf("fail: got %d wanted %d\n", getSample(s, j, i), cs);

            cs++;
        }
    destroyDataset(s);
}
