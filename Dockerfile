FROM nuisancemc/tutorial:nuint2024

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

ADD start /usr/bin/
COPY notebooks ${HOME}/notebooks/
COPY interactive ${HOME}/interactive/
COPY interactive/README.md ${HOME}/
ADD iop_file_snapshot.sh ${HOME}/bin/
