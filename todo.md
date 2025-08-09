Features to Add : 

1. Styling Textbox
    a. Draw Text in Rectangle boundry defined by Width and Height. 
    b. Inner Text offset to scroll text content within rect area.
    c. Add options to Align text (Center, Left, Right) 
    d. Fill/NoFill Background
    e. Wrap Text Content By Character per line/ Words Per line / Strech to Rect Width 

2. Double Buffering on PostProcessing Stage. (Scale, Rotate, Mirror final frame before dispatch)

3. uCanvas Runtime Manager
    a. Allocating uCanvas Resources
    b. Event Monitor
    c. Script Parser

4. Import Resources Sprite, Images from SD/SPIFFS. Caching Data on RAM

5. Virtual Display Panel Over Websocket. Get Viewport in browser or, Python based app. (Need to confirm How optimal ESP32P4 is at Streaming Big sized buffer over ESP32C3 that connected through SDIO?)
