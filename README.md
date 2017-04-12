mixpanel-qt with buffered tracking

Qt library for mixpanel

Originally written by [jdauphant](https://github.com/jdauphant/mixpanel-qt). 

I added (thread safe) buffered tracking/engaging, as well as functionality to flush the buffer after a user-set time interval.

The library depends on qjson, which can be acquired [here](http://qjson.sourceforge.net/). To make it work, you may need to modify the link to qjson library in the src.pro file.

Tested on Qt 4.8.5

