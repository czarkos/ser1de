Git Repository for project Hermes' SERenaDE (ser1de)

Accelerator Configuration:

* Script: `scripts/configure_iaa_user.sh` `scripts/configure_dsa_user.sh`
* Command: 

```
sudo ./scripts/configure_iaa_user.sh 0 1,1, 16 8
sudo ./scripts/configure_dsa_user.sh 0 0,0, 16 4
```

Protobuf + QPL dependency setup:

* Script: `scripts/setup.sh`
* Commands:

```
git submodule update --init --recursive
./scripts/setup.sh
```
