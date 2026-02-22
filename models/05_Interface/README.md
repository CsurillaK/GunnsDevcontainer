Update subnetworks with `-m` (maintenance) flag to skip build:
```
python3 "${GUNNS_HOME}/draw/netexport.py" -m ./MixingNetwork.drawio
python3 "${GUNNS_HOME}/draw/netexport.py" -m ./MixingNetworkInterface.drawio
```

Create super network using a slightly modified `supcreate.py` from host OS (TKinter is unavailable in the container):
- Add DrawIO file selection dialog option next to XML.
```
python3 "${GUNNS_HOME}/draw/supcreate.py"
```

Export super network using a slightly modified `supcreate.py` from host OS (TKinter is unavailable in the container):
- Add DrawIO file selection dialog option next to XML.
```
python3 "${GUNNS_HOME}/draw/supexport.py"
```

Although the resulting python script is only compatible with Trick, it can still help with the C++ glue code between subnetworks.