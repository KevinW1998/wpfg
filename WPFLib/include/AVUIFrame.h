#pragma once

#include <AVUIContentControl.h>

namespace AVUI {
class RoutedCommand;
class RequestNavigateEventArgs;

class Frame : public ContentControl
{
public:
    Frame() { };
    ~Frame() { };

    DECLARE_ELEMENT(Frame, ContentControl);

    void Navigate(const String& navTo);
};


}; // namespace AVUI
