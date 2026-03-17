Park Projekt - OpenGL 3D Színtér
Ez egy C nyelven írt, SDL2 és Legacy OpenGL alapú 3D alkalmazás, amely egy faobjektumokkal benépesített parkot szimulál dinamikus bevilágítással és ködeffektussal.

Funkciók
Kamera és irányítás
FPS nézet: Euler-szögek alapú első személyű kamerakezelés.

Egér: Szabad körbenézés (Pitch és Yaw) függőleges korlátozással.

Mozgás: Nézési iránytól függő WASD alapú közlekedés.

Ütközés: Alapszintű kör alapú ütközésvizsgálat a fák törzsével.

Renderelés és látvány
Modellbetöltő: Egyedi .obj fájl olvasó, amely kezeli a csúcspontokat, textúra-koordinátákat és normálisokat.

Dinamikus bevilágítás: Időben változó napállás (körpályán mozgó fényforrás) állítható fényerővel.

Köd: Exponenciális (EXP2) ködeffektus a mélységérzet növeléséhez.

Skybox: Egyszerű gradiens alapú égbolt és talaj sík.

Használat
Irányítás
Egér: Nézelődés.

W / A / S / D: Mozgás.

+ / -: Fényerő növelése és csökkentése.

ESC: Kilépés.

Tervezett fejlesztések
Modern OpenGL: Áttérés shaderekre (GLSL) és VBO/VAO használatára a fix csővezeték helyett.

Textúrázás: A fák és a talaj tényleges textúrázása az anyagtulajdonságok alapján.

Fizika: Gravitáció és ugrás funkció implementálása.

Növényzet: Többféle növénytípus támogatása a színtérben.
