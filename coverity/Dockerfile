FROM ghcr.io/coti-io/coti-ledger-app-builder:latest

ARG PROJECT
ARG TOKEN

RUN apt-get install -qy wget

RUN wget -q https://scan.coverity.com/download/cxx/linux64 --post-data "token=$TOKEN&project=$PROJECT" -O cov-analysis-linux64.tar.gz && \
    mkdir /opt/cov-analysis-linux64 && \
    tar xzf cov-analysis-linux64.tar.gz --strip 1 -C /opt/cov-analysis-linux64 && \
    rm cov-analysis-linux64.tar.gz

ENV PATH=/opt/cov-analysis-linux64/bin:$PATH