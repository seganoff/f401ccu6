# f401ccu6 template

while in repos root dir, copy+rename src folder, keep it in root dir 
so libopencm3(+ freertos) can be found by makefiles(+ fillFolder.sh)
```
#from repos root dir run commands:
git checkout -b <projName> && \
rm libopencm3/gitClone rtos_sysroot/gitClone && \
git clone hXXps://github.com/libopencm3/libopencm3.git && make -C libopencm3/ TARGETS='stm32/f1 stm32/f4' && \
git clone hXXps://github.com/FreeRTOS/FreeRTOS-Kernel.git rtos_sysroot && \
cp -r ./src ./<projName> && cd <projName>/rtos && sh fillFolder.sh
# XX absolutely disgust when editors open up Browsers
```

git config core.sshCommand "ssh -i /pathTo/keygen/gh.priv" + chmod 600 gh.priv gh.pub
url = git@gh_com:<accName>/<repo>.git