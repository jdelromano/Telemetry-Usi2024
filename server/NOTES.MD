# Annotazioni di risvolti emersi durante lo sviluppo

L'applicazione UserFeedbackConsole richiede un url, che sarà fornito dal docker-compose.

Non si deve utilizzare Graphana ma Matomo, dal momento che il primo non offre tutte le funzionalità necessarie.

Ad ogni release si devono rimuovere le info sulla release precedente, ma al contempop mantenere i dati riguardanti gli utilizzi.
Possiamo provare a utilizzare due survey diversi, con gli url che vengono shippati insieme alla release.
