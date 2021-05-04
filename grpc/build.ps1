$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine")+";"+
[System.Environment]::GetEnvironmentVariable("Path","User")

if((Test-Path 'C:/Program Files (x86)/Microsoft Visual Studio/2019') -and
(Get-ChildItem 'C:/Program Files (x86)/Microsoft Visual Studio/2019' -Recurse | measure -Property length -Sum).Sum -gt 100000) {
  $vs_path='C:/Program Files (x86)/Microsoft Visual Studio/2019'
  $vs_version='Visual Studio 16 2019'
} elseif ((Test-Path 'C:/Program Files (x86)/Microsoft Visual Studio/2017') -and
(Get-ChildItem 'C:/Program Files (x86)/Microsoft Visual Studio/2017' -Recurse | measure -Property length -Sum).Sum -gt 100000) {
  $vs_path='C:/Program Files (x86)/Microsoft Visual Studio/2017'
  $vs_version='Visual Studio 15 2017 Win64'
} else {
  write-host "`nNot find compiler." -ForegroundColor Red
  exit
}

if(Test-Path "$vs_path/BuildTools") {
  $vs_path+='/BuildTools/VC/Auxiliary/Build'
} elseif (Test-Path "$vs_path/Professional") {
  $vs_path+='/Professional/VC/Auxiliary/Build'
} else {
  write-host "`nNot find compiler." -ForegroundColor Red
  exit
}

pushd $vs_path
cmd /c "vcvarsall.bat amd64&set" |
  foreach {
      if ($_ -match "=") {
          $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
      }
  }
popd
write-host "`n$vs_version Command Prompt variables set." -ForegroundColor Yellow

# change source path if needed
cmake -G Ninja "..\..\source\prototypes\grpc"
cmake --build . --config Debug
