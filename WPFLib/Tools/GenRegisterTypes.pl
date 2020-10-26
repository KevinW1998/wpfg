
sub HeaderFromFiles
{
   opendir(IMD, $_[0]) || die("Couldn't open directory");
   @dirFiles = readdir(IMD);
   closedir(IMD);
   
   foreach $f (@dirFiles)
   {
      if ($f ne "." && $f ne "..")
      {
         if($f =~ /.*\.cpp/ )
         {
            my $fileName = $_[0] . "\\" . $f;
            unless(-d $fileName)
            {
               open(F, $fileName) || die("Error opening file");
               my $refThis = 0;

               while ( $line = <F> ) 
               {
                  if($line =~ /REGISTER_ELEMENT\((.*)\)/ )
                  {
                     $refThis = 1;
                  }

                  if($line =~ /REGISTER_ELEMENT_BEGIN\((.*)\)/ )
                  {
                     $refThis = 1;
                  }

                  if($line =~ /REGISTER_ABSTRACT_TYPE\((.*)\)/ )
                  {
                     $refThis = 1;
                  }

                  if($line =~ /REGISTER_VALUE_TYPE\((.*)\)/ )
                  {
                     $refThis = 1;
                  }

                  if($line =~ /REGISTER_VALUE_TYPE_BEGIN\((.*)\)/ )
                  {
                     $refThis = 1;
                  }

               }

               close(F);

               if($refThis eq 1)
               {
                  if($f =~ /(.*)\.cpp/)
                  {
                     print("#include <" . $1 . ".h>\n");
                  }
               }
            }
         }
      }
   
   }   
}



sub TypesFromFiles
{
   opendir(IMD, $_[0]) || die("Couldn't open directory");
   @dirFiles = readdir(IMD);
   closedir(IMD);
   
   foreach $f (@dirFiles)
   {
      if ($f ne "." && $f ne "..")
      {
         if($f =~ /.*\.cpp/ )
         {
            my $fileName = $_[0] . "\\" . $f;
            unless(-d $fileName)
            {
               open(F, $fileName) || die("Error opening file");

               while ( $line = <F> ) 
               {
                  if($line =~ /REGISTER_ELEMENT\((.*)\)/ )
                  {
                     print "    RegisterType(GET_TYPE($1));\n";
                  }

                  if($line =~ /REGISTER_ELEMENT_BEGIN\((.*)\)/ )
                  {
                     print "    RegisterType(GET_TYPE($1));\n";
                  }

                  if($line =~ /REGISTER_ABSTRACT_TYPE\((.*)\)/ )
                  {
                     print "    RegisterType(GET_TYPE($1));\n";
                  }

                  if($line =~ /REGISTER_VALUE_TYPE\((.*)\)/ )
                  {
                     print "    RegisterType(GET_TYPE($1));\n";
                  }

                  if($line =~ /REGISTER_VALUE_TYPE_BEGIN\((.*)\)/ )
                  {
                     print "    RegisterType(GET_TYPE($1));\n";
                  }

               }

               close(F);
            }
         }
      }
   
   }   
}

print "#include <AVUICommon.h>\n\n";
print "#include <AVUIApplication.h>\n\n";


foreach $arg (@ARGV) 
{
   HeaderFromFiles($arg);
}

print "namespace AVUI {";


print "bool Application::RegisterTypes()\n";
print "{\n";
foreach $arg (@ARGV) 
{
   TypesFromFiles($arg);
}
print "    return true;\n";

print "}\n";

print "};\n";
