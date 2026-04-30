const HOME = process.env.HOME;

const CERT_BASE = `${HOME}/tmp/localhost-certs`;


export default {
  certPath: `${CERT_BASE}/localhost.crt`,
  keyPath: `${CERT_BASE}/localhost.key`,
  port: 2200,
};

