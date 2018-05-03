#ifndef _USERINTERFACE_H
#define _USERINTERFACE_H

class UserInterface 
{
    public:
        float                xRotation, yRotation, zRotation; 
        float                xTrans, yTrans, zTrans;

        bool                spin = false;
        bool                fill = true;
        bool                m_showimgui = true;
        bool                m_playanimation = false;

        // Model Info: Values are set during Rendering
        unsigned int        m_TotalNumIndices = 0;

        UserInterface();
        ~UserInterface();
};

#endif
