FROM nuisancemc/nuisance:Q2-2024

RUN apt-get install -y python3-pip nano
RUN python3 -m pip install --no-cache notebook

ADD start /usr/bin/
COPY notebooks /notebooks/
COPY interactive /interactive/
#ADD iop_file_snapshot.sh /opt/nuisance/bin/
