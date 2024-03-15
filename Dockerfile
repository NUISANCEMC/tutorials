FROM nuisancemc/nuisance:Q2-2024

RUN apt-get install -y python3-pip
RUN python3 -m pip install jupyter metakernel 

COPY start /usr/bin/
COPY notebooks /notebooks/

EXPOSE 8888
