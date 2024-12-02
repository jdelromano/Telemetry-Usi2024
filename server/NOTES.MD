# Annotazioni di risvolti emersi durante lo sviluppo

L'applicazione UserFeedbackConsole richiede un url, che sarà fornito dal docker-compose.

Non si deve utilizzare Graphana ma Matomo, dal momento che il primo non offre tutte le funzionalità necessarie.

Ad ogni release si devono rimuovere le info sulla release precedente, ma al contempo mantenere i dati riguardanti gli utilizzi.
Possiamo provare a utilizzare due survey diversi, con gli url che vengono shippati insieme alla release.

### Cosa sono i surveys
ogni survey ha un uuid, un url, un nome, un prodotto e una target expression.
I surveys **attivi** si possono ottenere tramite l'endpoint `http://localhost:1984/receiver/surveys`
Se la versione è vecchia prenderemo:
- Lingua
- Regione
- Tipo di licenza
- Sistema operativo
- Versione del sistema operativo


Come facciamo per il version number?
- version_major
- version_minor
- version_patch
- version_build

E se manca? che default? 0, 1 o None?
