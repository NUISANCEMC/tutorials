FROM nuisancemc/nuisance:Q2-2024

RUN apt-get install -y python3-pip
RUN python3 -m pip install --no-cache notebook

COPY start /usr/bin/
COPY notebooks /notebooks/
COPY interactive /interactive/
COPY iop_file_snapshot.sh /opt/nuisance/bin/

EXPOSE 8888
WORKDIR /
ENV HOME=/tmp
ARG NB_USER
ARG NB_UID
ENV USER ${NB_USER}
ENV HOME /home/${NB_USER}

RUN adduser --disabled-password \
    --gecos "Default user" \
    --uid ${NB_UID} \
    ${NB_USER}
WORKDIR ${HOME}

COPY start /usr/bin/
COPY notebooks ${HOME}/notebooks/
COPY interactive ${HOME}/interactive/
COPY iop_file_snapshot.sh /opt/nuisance/bin/
