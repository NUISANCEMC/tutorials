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

ADD --chown=${NB_USER} start /usr/bin/
COPY --chown=${NB_USER} notebooks ${HOME}/notebooks/
COPY --chown=${NB_USER} interactive ${HOME}/interactive/
COPY --chown=${NB_USER} interactive/README.md ${HOME}/
ADD --chown=${NB_USER} iop_file_snapshot.sh ${HOME}/
