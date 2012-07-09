#!/bin/bash

for Framework in $*; do
  cd "$Framework/Versions" 1> /dev/null 2> /dev/null
  if [ $? -eq 0 ]; then
    Name=`echo $Framework | sed 's/.framework//'`

    for Version in *; do
      if [ $Version != "Current" ]; then
        Framework_Path="@executable_path/../Resources/$Framework/$Name"
        echo "install_name_tool -id \"$Framework_Path\" \"$Name\""
        cd "$Version"
        chmod +w "$Name"
        #otool -l "$Name"
        install_name_tool -id "$Framework_Path" "$Name"
        #otool -l "$Name"
        chmod -w "$Name"
        cd ../
      fi
    done

    cd ../../

    for Referer in $*; do
      if [ $Referer != $Framework ]; then
        cd "$Referer/Versions" 1> /dev/null 2> /dev/null
        if [ $? -eq 0 ]; then
          RName=`echo $Referer | sed 's/.framework//'`
          #echo "  $Referer refers to $Framework?"

          for Version in *; do
            #echo "$Version"
            if [ $Version != "Current" ]; then
              cd "$Version"
              for Change in `otool -l "$RName" | grep "$Name" | grep -v "$RName"`; do
                Old_Path=`echo $Change | grep "$Name"`
                if [ "$Old_Path" != "" ] && [ "$Old_Path" != "$Framework_Path" ]; then
                  chmod +w "$RName"
                  echo "install_name_tool -change \"$Old_Path\" \"$Framework_Path\" \"$RName\""
                  install_name_tool -change "$Old_Path" "$Framework_Path" "$RName"
                  chmod -w "$RName"
                fi
              done
              cd ../
            fi
          done
        fi
      cd ../../
      fi
    done
  fi
done
