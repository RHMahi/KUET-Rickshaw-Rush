#include "../include/Campus.h"

namespace KUET {

// Building colors (KUET theme)
static const sf::Color cAcademic   (70,  130, 200);
static const sf::Color cAcademicRf (100, 150, 230);
static const sf::Color cHall       (200, 130,  60);
static const sf::Color cHallRf     (220, 160,  80);
static const sf::Color cGate       (139,  69,  19);
static const sf::Color cGateRf     (160,  90,  40);
static const sf::Color cGray       (110, 110, 110);
static const sf::Color cGrayRf     (140, 140, 140);
static const sf::Color cService    (160,  80,  80);
static const sf::Color cServiceRf  (190, 110, 110);
static const sf::Color cFemale     (210, 120, 160);
static const sf::Color cFemaleRf   (230, 150, 180);
static const sf::Color cMosque     (60,  140, 140);
static const sf::Color cMosqueRf   (80,  170, 170);
static const sf::Color cWhite      (230, 230, 230);
static const sf::Color cWhiteRf    (255, 255, 255);

std::vector<CampusBuilding> buildCampusBuildings() {
    std::vector<CampusBuilding> buildings;

    // LEFT EXTENSION AREA
    buildings.push_back({"Sub-Station",          55,  190,  60,  50, cGray,    cGrayRf,   false});
    buildings.push_back({"OHT (Water Tower)",    55,  130,  35,  55, cGray,    cGrayRf,   false});
    buildings.push_back({"Institute Building",   90,  360,  85,  85, cAcademic, cAcademicRf, true});
    buildings.push_back({"Hall (Female-1)",     140,  290,  80,  95, cFemale,  cFemaleRf, true});
    buildings.push_back({"Hall (Female-2)",     140,  420,  80,  95, cFemale,  cFemaleRf, false});
    buildings.push_back({"Hall (Male-1)",       200,  730,  85, 100, cHall,    cHallRf,   true});
    buildings.push_back({"Hall (Male-2)",       200,  875,  85, 100, cHall,    cHallRf,   false});
    buildings.push_back({"Dormitory (Foreign)", 140,  975,  90,  80, cHall,    cHallRf,   false});
    buildings.push_back({"TT&C Center",         265,  940, 120,  80, cGray,    cGrayRf,   false});
    buildings.push_back({"Staff Bungalow",      320, 1160,  90,  70, cHall,    cHallRf,   false});

    // NORTH CAMPUS
    buildings.push_back({"Staff Tower",         445,  195,  52,  85, cGray,    cGrayRf,   true});
    buildings.push_back({"KUET Library",       1120,  175, 155, 105, cGate,    cGateRf,   true});
    buildings.push_back({"KUET School",        1560,  210, 125,  95, cAcademic, cAcademicRf, true});
    buildings.push_back({"Guest / VIP House",  1700,  230,  90,  70, cGate,    cGateRf,   true});

    // ME && ROKEYA AREA
    buildings.push_back({"ME Building",         930,  460, 115,  90, cAcademic, cAcademicRf, true});
    buildings.push_back({"Rokeya Hall (Female)",1240,  415, 120, 100, cFemale, cFemaleRf, true});
    buildings.push_back({"VP Staff Residence",  1460,  415, 105,  85, cGate,   cGateRf,   false});
    buildings.push_back({"Textile Workshop",    660,   640, 115, 100, cGray,   cGrayRf,   false});
    buildings.push_back({"Garage / Workshop",   805,   620, 100,  90, cGray,   cGrayRf,   false});

    // CENTRAL CAMPUS
    buildings.push_back({"Medical Center",      755,   735, 100,  75, cService, cServiceRf, true});
    buildings.push_back({"CSE Building",        935,   730, 115,  90, cAcademic, cAcademicRf, true});
    buildings.push_back({"CE Building",        1130,   725, 115,  90, cAcademic, cAcademicRf, false});
    buildings.push_back({"EEE Building",       1250,  1070, 125,  95, cAcademic, cAcademicRf, true});
    buildings.push_back({"NAB-D",               840,   940,  70, 100, cAcademic, cAcademicRf, false});
    buildings.push_back({"NAB-C",               920,   940,  70, 100, cAcademic, cAcademicRf, false});
    buildings.push_back({"NAB-B",              1000,   940,  70, 100, cAcademic, cAcademicRf, false});
    buildings.push_back({"NAB-A",              1130,   940,  70, 100, cAcademic, cAcademicRf, false});
    buildings.push_back({"Central Computer Center", 1215, 945, 110, 85, cAcademic, cAcademicRf, true});
    buildings.push_back({"URP Building",        900,  1050, 100,  80, cAcademic, cAcademicRf, false});
    buildings.push_back({"Auditorium",         1080,  1050, 100,  85, cGate,    cGateRf,   true});

    // STUDENT WELFARE
    buildings.push_back({"Sadar Mosque",        845,  1140,  80,  70, cMosque,  cMosqueRf, true});
    buildings.push_back({"New Academic Bldg",   940,  1140, 130,  90, cAcademic, cAcademicRf, false});
    buildings.push_back({"July Chattar",        1130,  1170,  90,  80, cWhite,   cWhiteRf,  false});
    buildings.push_back({"Shaheed Minar",      1445,  1160,  52,  80, cWhite,   cWhiteRf,  false});
    buildings.push_back({"SWC Building",       1215,  1295, 125,  95, cAcademic, cAcademicRf, true});
    buildings.push_back({"ATM / Booth",        1205,  1345,  50,  45, cGray,    cGrayRf,   false});
    buildings.push_back({"Extension (Proposed)",1605, 1295, 130, 100, cGray,    cGrayRf,   false});

    // Halls (moved below the hall road so the road stays fully drivable)
    buildings.push_back({"Lalon Shah Hall",     535,  1300, 115, 100, cHall,   cHallRf,   true});
    buildings.push_back({"Amar Ekushey Hall",   680,  1300, 115, 100, cHall,   cHallRf,   true});
    buildings.push_back({"Shaheed Smriti Hall", 835,  1300, 110, 100, cHall,   cHallRf,   true});
    buildings.push_back({"Rashid Hall",         985,  1310,  95,  85, cHall,   cHallRf,   false});

    // Gates
    buildings.push_back({"KUET Main Gate",     1805,  1370,  85,  55, cGate,   cGateRf,   false});
    buildings.push_back({"KUET 2nd Gate",       755,  1440,  85,  45, cGate,   cGateRf,   false});
    buildings.push_back({"Pocket Gate (South)", 505,  1440,  70,  40, cGate,   cGateRf,   false});
    buildings.push_back({"Pocket Gate (North)", 1740,   50,  70,  40, cGate,   cGateRf,   false});

    return buildings;
}

std::vector<CampusRoad> buildCampusRoads() {
    std::vector<CampusRoad> roads;

    roads.push_back({     0, 1400, 2000,     80, false }); // bottom main road
    roads.push_back({   400,   50, 1380,     65, false }); // top road
    roads.push_back({  1800,   50, 70,    1400, true  }); // right road
    roads.push_back({   385,    0, 65,    1500, true  }); // left road
    roads.push_back({   620,  825, 700,     60, false }); // central horizontal
    roads.push_back({  1060,  340, 60,     680, true  }); // central vertical
    roads.push_back({   385,  560, 300,     55, false }); // left horizontal
    roads.push_back({   450, 1240, 680,     55, false }); // hall road
    roads.push_back({  1380,  340, 60,     900, true  }); // right-mid vertical (tall, reaches the connector)
    roads.push_back({   390,  380, 720,     55, false }); // upper-mid horizontal
    roads.push_back({  1060,  285, 400,     60, false }); // north connector (Library / north campus)

    return roads;
}

}