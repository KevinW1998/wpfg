use Win32::OLE qw(in with);
use Data::Dumper;

my $file = $ARGV[0];
my $dom = Win32::OLE->new('MSXML2.DOMDocument.3.0');

$dom->{async} = "False";
$dom->{validateOnParse} = "False";
$dom->Load($file) or die "Parse failed";

my $enums = $dom->selectNodes("//Enum");

print "#pragma once\n";
print "#include <AVUIEnumClass.h>\n\n\n";
print "namespace AVUI {\n";

foreach my $node (in $enums)
{
   $nodeName = $node->Attributes->getNamedItem("Name")->{Text};
   $enumValues = $node->selectNodes("EnumValue");

   print "class " . $nodeName . " : public EnumClass\n";
   print "{\n";

   print "public:\n";
   print "    enum Enum\n";
   print "    {\n";

   foreach my $enumValue (in $enumValues)
   {
      $valueName = $enumValue->Attributes->getNamedItem("Name")->{Text};

      print "        " . $valueName;

      if(defined($enumValue->Attributes->getNamedItem("Value")))
      {
         $valueVal = $enumValue->Attributes->getNamedItem("Value")->{Text};
         print " = " . $valueVal;
      }
      
      print ",\n";
   }


   print "    };\n\n";

   print "    " . $nodeName . "() : EnumClass(GET_TYPE(".$nodeName.")) { };\n";
   print "    ~" . $nodeName . "() { };\n";

   print "    private:\n";
   print "    static const EnumClass::EnumStringMapping cs_rgMapping[];";
   print "    static const unsigned int cs_cMapping;";

   print "    virtual const EnumClass::EnumStringMapping* get_Mapping() const { return cs_rgMapping; }\n";
   print "    virtual unsigned int get_MappingCount() const { return cs_cMapping; }\n";

   print "\n\n";
   print "};\n\n";
}

print "\n};// Namespace AVUI\n";
