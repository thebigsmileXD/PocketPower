#include "PistonArmTile.h"
#include "PistonBaseTile.h"
#include "mcpe/world/level/TileSource.h"
#include "mcpe/world/entity/player/Player.h"
#include "mcpe/world/Facing.h"
#include "mcpe/world/phys/AABB.h"
#include "mcpe/world/material/Material.h"

PistonArmTile::PistonArmTile(int blockId) : Tile(blockId, &Material::stone) {
	init();
	
	setDestroyTime(0.5F);
	setNameId("piston_extension");
	renderType = 17;
	creativeTab = CreativeTab::ITEMS;
	Tile::solid[blockId] = false;
	Tile::lightBlock[blockId] = 0;

	tex = getTextureUVCoordinateSet("piston_top_normal", 0);
	texture_sticky = getTextureUVCoordinateSet("piston_top_sticky", 0);
	texture_side = getTextureUVCoordinateSet("piston_side", 0);
}

void PistonArmTile::playerDestroy(Player* player, int x, int y, int z, int side) {
	// Remove the attached piston base
	int data = player->region.getData(x, y, z);
	int rotation = Facing::OPPOSITE_FACING[getRotation(data)];
	x += Facing::STEP_X[rotation];
	y += Facing::STEP_Y[rotation];
	z += Facing::STEP_Z[rotation];
	int tile = player->region.getTile(x, y, z).id;
	if(tile == Tile::pistonNormal->id || tile == Tile::pistonSticky->id) {
		data = player->region.getData(x, y, z);
		if(PistonBaseTile::isPowered(data)) {
			player->region.setTileAndData(x, y, z, {0, 0}, 0);
		}
	}
}

int PistonArmTile::getResourceCount(Random*, int, int) {
	return 0;
}

const AABB& PistonArmTile::getVisualShape(TileSource* region, int x, int y, int z, AABB& shape, bool b) {
	switch(getRotation(region->getData(x, y, z))) {
	case 0:
		shape.set(0.0F, 0.0F, 0.0F, 1.0F, 0.25F, 1.0F);
		break;
	case 1:
		shape.set(0.0F, 0.75F, 0.0F, 1.0F, 1.0F, 1.0F);
		break;
	case 2:
		shape.set(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.25F);
		break;
	case 3:
		shape.set(0.0F, 0.0F, 0.75F, 1.0F, 1.0F, 1.0F);
		break;
	case 4:
		shape.set(0.0F, 0.0F, 0.0F, 0.25F, 1.0F, 1.0F);
		break;
	case 5:
		shape.set(0.75F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F);
		break;
	}
	
	return shape;
}

void PistonArmTile::neighborChanged(TileSource* region, int x, int y, int z, int xx, int yy, int zz) {
	int rotation = getRotation(region->getData(x, y, z));
	Tile* tile = Tile::tiles[region->getTile(x - Facing::STEP_X[rotation], y - Facing::STEP_Y[rotation], z - Facing::STEP_Z[rotation]).id];
	if(tile != Tile::pistonNormal && tile != Tile::pistonSticky)
		region->setTileAndData(x, y, z, {0, 0}, 3);
	else
		tile->neighborChanged(region, x - Facing::STEP_X[rotation], y - Facing::STEP_Y[rotation], z - Facing::STEP_Z[rotation], xx, yy, zz);
}

const TextureUVCoordinateSet& PistonArmTile::getTexture(signed char side, int data) {
	int rotation = getRotation(data);
	if(side == rotation)
		return isSticky(data)? texture_sticky : tex;
	int opposite[6] = {1, 0, 3, 2, 5, 4};
	if(side == opposite[rotation])
		return tex;
	return texture_side;
}

int PistonArmTile::getRotation(int data) {
	return data & 7;
}

bool PistonArmTile::isSticky(int data) {
    int sticky = data & 8;
    if(sticky == 8) return true;
    if(sticky == 0) return false;
    return false;
}

bool PistonArmTile::addCollisionShapes(TileSource& region, int x, int y, int z, AABB const* posAABB, std::vector<AABB, std::allocator<AABB>>& pool) {
	int data = region.getData(x, y, z);
	float var9 = 0.25F;
	float var10 = 0.375F;
	float var11 = 0.625F;
	float var12 = 0.25F;
	float var13 = 0.75F;

	switch(getRotation(data)) {
    	case 0:
    	    addAABB(AABB(0.0F, 0.0F, 0.0F, 1.0F, 0.25F, 1.0F).move(x, y, z), posAABB, pool);
    	    addAABB(AABB(0.375F, 0.25F, 0.375F, 0.625F, 1.0F, 0.625F).move(x, y, z), posAABB, pool);
    	    break;
    	case 1:
    	    addAABB(AABB(0.0F, 0.75F, 0.0F, 1.0F, 1.0F, 1.0F).move(x, y, z), posAABB, pool);
    	    addAABB(AABB(0.375F, 0.0F, 0.375F, 0.625F, 0.75F, 0.625F).move(x, y, z), posAABB, pool);
    	    break;
    	case 2:
    	    addAABB(AABB(0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.25F).move(x, y, z), posAABB, pool);
    	    addAABB(AABB(0.25F, 0.375F, 0.25F, 0.75F, 0.625F, 1.0F).move(x, y, z), posAABB, pool);
    	    break;
    	case 3:
    	    addAABB(AABB(0.0F, 0.0F, 0.75F, 1.0F, 1.0F, 1.0F).move(x, y, z), posAABB, pool);
    	    addAABB(AABB(0.25F, 0.375F, 0.0F, 0.75F, 0.625F, 0.75F).move(x, y, z), posAABB, pool);
    	    break;
    	case 4:
    	    addAABB(AABB(0.0F, 0.0F, 0.0F, 0.25F, 1.0F, 1.0F).move(x, y, z), posAABB, pool);
    	    addAABB(AABB(0.375F, 0.25F, 0.25F, 0.625F, 0.75F, 1.0F).move(x, y, z), posAABB, pool);
    	    break;
    	case 5:
    	    addAABB(AABB(0.75F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F).move(x, y, z), posAABB, pool);
    	    addAABB(AABB(0.0F, 0.375F, 0.25F, 0.75F, 0.625F, 0.75F).move(x, y, z), posAABB, pool);
    	    break;
    }
	return true;
}

void PistonArmTile::setOverrideTexture(TextureUVCoordinateSet texture) {
	overrideTexture = texture;
}
