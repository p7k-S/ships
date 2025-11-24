void Game::createTroops() {
    std::vector<gl::Hex*> waterCells;
    for (auto& hex : hexMap) {
        if (hex.getCellType() == gl::CellType::WATER && portCanPlayced(hex)) {
            waterCells.push_back(&hex);
        }
    }

    std::vector<std::vector<gl::Hex*>> sectors(playersAmount);
    for (size_t i = 0; i < waterCells.size(); ++i) {
        int sector = (i * playersAmount) / waterCells.size();
        sectors[sector].push_back(waterCells[i]);
    }

    std::random_device rd;
    std::mt19937 g(rd());

    for (int i = 0; i < playersAmount; ++i) {
        if (!sectors[i].empty()) {
            std::shuffle(sectors[i].begin(), sectors[i].end(), g);
            gl::Hex* selectedHex = sectors[i][0];

            std::cout << "DEBUG: Creating port for player " << i 
                << " at (" << selectedHex->q << "," << selectedHex->r << ")" << std::endl;
            std::cout << "DEBUG: Hex already has building: " << selectedHex->hasBuilding() << std::endl;
            std::cout << "DEBUG: Hex already has troop: " << selectedHex->hasTroop() << std::endl;

            gl::Owner owner = players[i].get();
            auto port = std::make_unique<gl::Port>(owner, selectedHex);
            gl::Port* portPtr = port.get();

            if (selectedHex->setBuildingOf<gl::Port>(portPtr)) {
                players[i]->addBuilding(std::move(port));

                auto ship = std::make_unique<gl::Ship>(owner, selectedHex);
                gl::Ship* shipPtr = ship.get();

                if (selectedHex->setTroopOf<gl::Ship>(shipPtr)) {
                    players[i]->addTroop(std::move(ship));
                    std::cout << "DEBUG: Player " << i << " now has " << players[i]->getTroops().size() << " troops" << std::endl;

                    addViewedCells(players[i]->getSeenCells(), shipPtr, hexMap, gl::RangeMode::VIEW);
                    // это нужно т.к. хост создает и id всегда хостовое
                    addViewedCells(players[i]->getViewableCells(), shipPtr, hexMap, gl::RangeMode::VIEW);
                }

                sectors[i].erase(sectors[i].begin());
            } else {
                std::cout << "DEBUG: FAILED to create port for player " << i << std::endl;
            }
        } else {
            std::cout << "DEBUG: No cells for player " << i << std::endl;
        }
    }

    // 1. СОБИРАЕМ все оставшиеся клетки из sectors
    std::vector<gl::Hex*> remainingCells;
    for (auto& sector : sectors) {
        remainingCells.insert(remainingCells.end(), sector.begin(), sector.end());
    }

    // 2. ВЫЧИСЛЯЕМ количество секторов для NPC
    size_t npcSectorCount = static_cast<size_t>(remainingCells.size() * percent_ships_in_water);
    npcSectorCount = std::max(size_t(1), std::min(npcSectorCount, remainingCells.size()));

    std::vector<std::vector<gl::Hex*>> npcSectors(npcSectorCount);
    for (size_t i = 0; i < remainingCells.size(); ++i) {
        int sector = (i * npcSectorCount) / remainingCells.size();
        npcSectors[sector].push_back(remainingCells[i]);
    }


    for (int i = 0; i < npcSectors.size(); ++i) { // *2 тк к каждому в его сектор добавим по enemy
            std::shuffle(npcSectors[i].begin(), npcSectors[i].end(), g);
            gl::Hex* selectedHex = npcSectors[i][0];

            gl::Owner owner = static_cast<gl::Owner>(&enemy);
            auto port = std::make_unique<gl::Port>(owner, selectedHex);
            gl::Port* portPtr = port.get();
            if (selectedHex->setBuildingOf<gl::Port>(portPtr)) {
                enemy.addBuilding(std::move(port));
                npcSectors[i].erase(npcSectors[i].begin());
            }
    }


    std::vector<gl::Hex*> allWaterCells;
    for (auto& hex : hexMap) {
        if (hex.getCellType() == gl::CellType::DEEPWATER || hex.getCellType() == gl::CellType::WATER) {
            if (!hex.hasTroop())
                allWaterCells.push_back(&hex);
        }
    }

    std::shuffle(allWaterCells.begin(), allWaterCells.end(), g);

    for (size_t i = playersAmount; i - playersAmount < allWaterCells.size() * percent_ships_in_water; ++i) {
        gl::Owner owner = (i % 2) ? static_cast<gl::Owner>(&pirate) : static_cast<gl::Owner>(&enemy);
        auto ship = std::make_unique<gl::Ship>(owner, allWaterCells[i]);
        gl::Ship* shipPtr = ship.get();
        allWaterCells[i]->setTroopOf<gl::Ship>(shipPtr);

        if (i % 2) {
            pirate.addTroop(std::move(ship));
        } else {
            enemy.addTroop(std::move(ship));
        }

        allWaterCells.erase(allWaterCells.begin());
    }
}
