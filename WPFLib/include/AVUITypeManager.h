namespace AVUI {

class DependencyObject;
class DependencyProperty;

class String;

class TypeManager
{
public:
    friend class Type;

    static Type* FindType(const String& type);

    static void RegisterType(Type* pType);
    static const DependencyProperty* GetDependencyPropertyByID(unsigned int globalID);

    static void CleanupAllocatedData();

private:
    static Type* ms_pTypeFirst;
};

};
