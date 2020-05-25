
const char* genFileFirstHeader = R"(
// NOTE: This code is automatically generated by meta/entitiesParser.exe 

#include "pch.hpp"
#include "entitiesDebugger.hpp"
#include "Renderer/renderer.hpp"

)";

const char* genFileSecondHeader = R"(

extern bool debugWindowOpen;

namespace ph::system {

constexpr unsigned lookForSize = 255;
static char lookFor[lookForSize];
static bool highlightSelected = true;

static char* components[] = {
	"Health", "Damage", "Player", "Killable", "InPlayerGunAttackArea"
};
static bool selectedComponents[IM_ARRAYSIZE(components)];

static bool selectingInWorldMode = false;
static float selectingInWorldModeF4InputDelay = 0.f;
static unsigned selectingInWorldModeZPriority = 0;
static float selectingInWorldModeZPriorityInputDelay = 0.f;

EntitiesDebugger::EntitiesDebugger(entt::registry& reg, sf::Window* window)
	:System(reg)
	,mWindow(window)
{
}

static unsigned getCharCount(char* str, size_t size)
{
	for(unsigned charCount = 0; charCount < static_cast<unsigned>(size); ++charCount)
		if(str[charCount] == 0)
			return charCount;
	return static_cast<unsigned>(size);
}

void EntitiesDebugger::update(float dt)
{
	PH_PROFILE_FUNCTION();	

	if(debugWindowOpen && ImGui::BeginTabItem("entities debugger"))
	{
		ImGui::BeginChild("entities", ImVec2(360, 0), true);
		ImGui::Checkbox("hightlight selected", &highlightSelected);

		if(selectingInWorldModeF4InputDelay > 0.f)
			selectingInWorldModeF4InputDelay -= dt;

		if(selectingInWorldMode)
		{
			if(ImGui::Button("leave world entity seleting [F4]"))
			{
				selectingInWorldMode = false;
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::F4) && selectingInWorldModeF4InputDelay <= 0.f)
			{	
				selectingInWorldModeF4InputDelay = 0.4f;
				selectingInWorldMode = false;
			}
		}
		else
		{
			if(ImGui::Button("select entity in world [F4]"))
			{
				selectingInWorldMode = true;
			}
			else if(sf::Keyboard::isKeyPressed(sf::Keyboard::F4) && selectingInWorldModeF4InputDelay <= 0.f)
			{	
				selectingInWorldModeF4InputDelay = 0.4f;
				selectingInWorldMode = true;
			}
		}

		if(selectingInWorldMode)
		{
			if(selectingInWorldModeZPriorityInputDelay > 0.f)
				selectingInWorldModeZPriorityInputDelay -= dt;

			sf::Vector2f currentCamTopLeft;
			sf::Vector2f currentCamSize;
			mRegistry.view<component::Camera>().each([&]
			(auto camera)
			{
				if(camera.name == component::Camera::currentCameraName)
				{
					currentCamTopLeft = camera.center() - camera.getSize() / 2.f;
					currentCamSize = camera.getSize();
				}
			});

			auto mouseWindowPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*mWindow));
			auto resolutionRatio = Math::hadamardDiv(currentCamSize, static_cast<sf::Vector2f>(mWindow->getSize()));
			auto mouseWorldPos = (Math::hadamardMul(mouseWindowPos, resolutionRatio)) + currentCamTopLeft; 

			struct EntityUnderCursor
			{
				entt::entity entity;
				unsigned char z;
			};
			std::vector<EntityUnderCursor> entitiesUnderCursor;

			auto bodiesView = mRegistry.view<component::BodyRect>();
			for(auto entity : bodiesView)
			{
				const auto& body = bodiesView.get<component::BodyRect>(entity); 
				if(body.contains(mouseWorldPos))
				{
					EntityUnderCursor euc;
					euc.entity = entity;

					if(auto* rq = mRegistry.try_get<component::RenderQuad>(entity))
						euc.z = rq->z; 
					else if(auto* rc = mRegistry.try_get<component::RenderChunk>(entity))
						euc.z = rc->z;
					else if(auto* grc = mRegistry.try_get<component::GroundRenderChunk>(entity))
						euc.z = grc->z;
					else
						euc.z = 255;

					entitiesUnderCursor.emplace_back(euc);
				}
			}

			std::sort(entitiesUnderCursor.begin(), entitiesUnderCursor.end(), []
			(const auto& a, const auto& b)
			{
				return a.z < b.z;
			});

			if(selectingInWorldModeZPriority > entitiesUnderCursor.size() - 1)
				selectingInWorldModeZPriority = 0; 

			ImGui::Text("left mouse button - select entity");
			ImGui::Text("right mouse button - leave world entity seleting");
			ImGui::Text("middle mouse button - change z priority");

			if(entitiesUnderCursor.size())
			{
				auto underCursorEntity = entitiesUnderCursor[selectingInWorldModeZPriority].entity;
				auto underCursorZ = entitiesUnderCursor[selectingInWorldModeZPriority].z;

				unsigned char alpha = underCursorEntity == mSelected ? 45 : 150;
				auto& body = mRegistry.get<component::BodyRect>(underCursorEntity);
				Renderer::submitQuad(nullptr, nullptr, &sf::Color(255, 0, 0, alpha), nullptr,
					body.pos, body.size, underCursorZ, 0.f, {}, ProjectionType::gameWorld, false);

				for(unsigned i = 1; i < entitiesUnderCursor.size(); ++i)
				{
					auto& body = mRegistry.get<component::BodyRect>(underCursorEntity);
					Renderer::submitQuad(nullptr, nullptr, &sf::Color(255, 0, 0, 40), nullptr,
						body.pos, body.size, underCursorZ, 0.f, {}, ProjectionType::gameWorld, false);
				}

				if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
				{
					mSelected = underCursorEntity; 
				}

				if(sf::Mouse::isButtonPressed(sf::Mouse::Middle) && selectingInWorldModeZPriorityInputDelay <= 0.f)
				{
					selectingInWorldModeZPriorityInputDelay = 0.2f;
					if(selectingInWorldModeZPriority < entitiesUnderCursor.size() - 1)
						++selectingInWorldModeZPriority;
					else
						selectingInWorldModeZPriority = 0;
				}
			}

			if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				selectingInWorldMode = false;	
			}
		}
		else
		{
			selectingInWorldModeZPriorityInputDelay = 0.f;
		}

		/* TODO: Add support for components choosing
		if(ImGui::TreeNode("choose components"))
		{
			if(ImGui::ListBoxHeader("comListBox"))
			{	
				for(int i = 0; i < IM_ARRAYSIZE(components); ++i)			
				{
					ImGui::Selectable(components[i], selectedComponents + i);
				}
				ImGui::ListBoxFooter();
			}
			ImGui::TreePop();
		}
		*/

		ImGui::InputText("debug name", lookFor, lookForSize);

		unsigned lookForCharCount = getCharCount(lookFor, lookForSize);

		auto selectableEntity = [=](entt::entity entity)
		{
			bool displayThisEntity = true;
			char label[50];
			if(auto* debugName = mRegistry.try_get<component::DebugName>(entity))
			{
				char* name = debugName->name;
				unsigned nameCharCount = getCharCount(name, strlen(name));
				sprintf(label, "%u - %s", entity, name);
				if(lookForCharCount != 0 && lookFor[0] != ' ')
				{
					for(unsigned i = 0; i <= nameCharCount && i < lookForCharCount; ++i)
					{
						char nameChar = name[i];
						char lookForChar = lookFor[i]; 
						if(nameChar != lookForChar)
						{
							if(lookForChar > 96 && lookForChar < 123)
							{
								if(lookForChar - 32 != nameChar)
								{
									displayThisEntity = false;
									break;
								}
							}
							else
							{
								displayThisEntity = false;
								break;
							}
						}
					}
				}
			}
			else if(lookFor[0] == ' ')
			{
				sprintf(label, "%u", entity);
			}
			else
			{
				displayThisEntity = false;
			}

			if(displayThisEntity && ImGui::Selectable(label, mSelected == entity))
			{
				mSelected = entity;
			}
		};

		std::vector<entt::component> types;

		/* TODO: Add support for components choosing
		if(selectedComponents[0]) types.emplace_back(mRegistry.type<component::Health>());
		if(selectedComponents[1]) types.emplace_back(mRegistry.type<component::Damage>());
		if(selectedComponents[2]) types.emplace_back(mRegistry.type<component::Player>());
		if(selectedComponents[3]) types.emplace_back(mRegistry.type<component::Killable>());
		if(selectedComponents[4]) types.emplace_back(mRegistry.type<component::InPlayerGunAttackArea>());

		if(types.empty() || types.size() == IM_ARRAYSIZE(components))
		{
			mRegistry.each([=](auto entity)
			{
				selectableEntity(entity);
			});
		}
		else
		{
			mRegistry.runtime_view(types.cbegin(), types.cend()).each([=](auto entity)
			{
				selectableEntity(entity);
			});	
		}
		*/

		ImGui::BeginChild("entities2", ImVec2(0, 0), true);
			mRegistry.each([=](auto entity)
			{
				selectableEntity(entity);
			});
		ImGui::EndChild();

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("components view");

		if(auto* debugName = mRegistry.try_get<component::DebugName>(mSelected))
			ImGui::Text("%s Components view:", debugName->name);
		else
			ImGui::Text("Components view:");

		if(mRegistry.valid(mSelected))
		{
			bool bodyValid = false;
			component::BodyRect body;	

			// NOTE: Change value to 0 if this code doesn't compile
			//       because of bug in components parser!
			//       If that happens you can make a commit with #if value set to 0
			//       Make sure to inform Czapa about bug in parser. 
			#if 1

			// HEADER ENDS

)";

const char* genFileFooter = R"(

			// FOOTER STARTS

			#endif // !COMPONENTS_PARSER_MESSED_UP

			if(highlightSelected && bodyValid)
			{
				Renderer::submitQuad(nullptr, nullptr, &sf::Color(255, 0, 0, 150), nullptr, body.pos, body.size,
									 10, 0.f, {}, ProjectionType::gameWorld, false);
			}
		}

		ImGui::EndChild();
		ImGui::EndTabItem();
	}
	else
	{
		selectingInWorldMode = false;
	}
}

}
)";

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <windows.h>

#define cast static_cast

using u32 = uint32_t;
using u8 = uint8_t;

struct Arena
{
	u32 used;
	u32 size;
	u8* base;

	Arena()
	{
		used = 0;
		size = 10000;
		base = cast<u8*>(calloc(10000, 1));
	}
};

Arena arena;

void* push(u32 size)
{
	if(arena.size < arena.used + size)
	{
		u32 newSize = arena.size + size * 2;
		arena.base = cast<u8*>(realloc(arena.base, size));
		arena.size = size;
	}
	void* res = cast<void*>(arena.base + arena.used);
	arena.used += size;
	return res;
}

void pop(u32 size)
{
	arena.used -= size;
}

struct Enums;

struct Enum
{
	u32 nrOfEnumerations = 0;
	char enumerations[50][50] = {};
	char name[50] = {};
	bool isEnumClass;
};

struct Enums
{
	Enum enums[10] = {};
	u32 enumsUsed = 0;
};

void addEnumeration(Enum* e, char* str)
{
	char* enumeration = e->enumerations[e->nrOfEnumerations]; 
	memcpy(enumeration, str, strlen(str));
	e->nrOfEnumerations++;
}

Enum* addEnum(Enums* enums, char* name, bool isEnumClass)
{
	auto& e = enums->enums[enums->enumsUsed];
	enums->enumsUsed++;
	u32 nameLength = strlen(name);
	memcpy(e.name, name, nameLength);
	e.name[nameLength] = 0;
	e.isEnumClass = isEnumClass;
	return &e; 
}

bool match(char* a, char* b)
{
	for(u32 i = 0; i < strlen(b); ++i)
		if(a[i] != b[i])
			return false;
	return true;
}

bool isAlpha(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

void parseComponentsFile(char* filename, FILE* genFile)
{
	char filepath[255];
	sprintf(filepath, "../src/ECS/Components/%s", filename);
	FILE* componentsFile = fopen(filepath, "r");
	if(!componentsFile)
	{
		printf("Failed to open components file \"%s\"\n", filepath); 
		return;
	}

	fseek(componentsFile, 0, SEEK_END);	
	u32 componentsFileLength = ftell(componentsFile);
	rewind(componentsFile);
	char* code = cast<char*>(push(componentsFileLength));
	fread(code, 1, componentsFileLength, componentsFile);

	{
		// advance code after namespace ph::component or namespace component
		char* codeCopy = code;
		code = strstr(code, "namespace ph::component");
		if(code)
		{
			code += strlen("namespace ph::component");
		}
		else
		{
			code = strstr(codeCopy, "namespace component");
			if(code)
			{
				code += strlen("namespace component");
			}
			else
			{
				printf("Failed to find namespace ph::component and namespace component");
				return;
			}
		}
		while(*code++ != '{');
	}

	while(*code++)
	{
		if(match(code, "struct"))
		{
			code += 6;	
			while(*code == ' ') ++code;
			char* componentName = code;
			while(isAlpha(*code)) ++code;
			*code = 0;
			++code;

			{
				// handle @no-debugger
				bool noDebugger = false;
				char* code2 = code;
				while(*code2++)
				{
					if(*code2 == '{') break;
					if(match(code2, "@no-debugger"))
					{
						noDebugger = true;
						while(!match(code, "};")) ++code;
						break;
					}
				}
				if(noDebugger) continue;
			}

			{
				// handle empty structs
				char* code2 = code;
				while(*code2 != '{') ++code2;
				bool structIsEmpty = false;
				while(*code2++)
				{
					if(isAlpha(*code2)) break;
					if(*code2 == '}')
					{
						structIsEmpty = true;
						fprintf(genFile, "if(mRegistry.has<component::%s>(mSelected)) \n{\n", componentName);
						fprintf(genFile, "ImGui::Separator();\n");
						fprintf(genFile, "ImGui::BulletText(\"%s\");\n}\n", componentName);
						while(*code2++ != ';');
						code = code2;
						break;
					}
				}
				if(structIsEmpty) continue;
			}

			fprintf(genFile, "if(auto* c = mRegistry.try_get<component::%s>(mSelected)) \n{\n", componentName);
			fprintf(genFile, "ImGui::Separator();\n");

			bool componentUsesInheritance = false;
			while(*code)
			{
				if(*code == '{') 
				{
					fprintf(genFile, "ImGui::BulletText(\"%s\");\n", componentName);
					break;
				}

				if(*code == ':')
				{
					// handle inheritance

					componentUsesInheritance = true;

					while(*code++)
					{
						if(isAlpha(*code))
						{
							if(match(code, "public")) 
							{
								code += 6;
								continue;
							}

							if(match(code, "FloatRect"))
							{
								code += 9;
								if(match(componentName, "BodyRect"))
								{
									fprintf(genFile, "body = *c;\nbodyValid = true;\n");
								}
								fprintf(genFile, "ImGui::BulletText(\"%s: %%f, %%f, %%f, %%f\", c->x, c->y, c->w, c->h);\n", componentName);
								while(*code++ != '{');
								break;
							}
							if(match(code, "IntRect"))
							{
								code += 7;
								fprintf(genFile, "ImGui::BulletText(\"%s: %%i, %%i, %%i, %%i\", c->x, c->y, c->w, c->h);\n", componentName);
								while(*code++ != '{');
								break;
							}
							if(match(code, "sf::Vector2f"))
							{
								code += 12;
								fprintf(genFile, "ImGui::BulletText(\"%s: %%f, %%f\", c->x, c->y);\n", componentName);
								while(*code++ != '{');
								break;
							}
							if(match(code, "sf::Vector2i"))
							{
								code += 12;
								fprintf(genFile, "ImGui::BulletText(\"%s: %%i, %%i\", c->x, c->y);\n", componentName);
								while(*code++ != '{');
								break;
							}
							if(match(code, "sf::Vector2u"))
							{
								code += 12;
								fprintf(genFile, "ImGui::BulletText(\"%s: %%u, %%u\", c->x, c->y);\n", componentName);
								while(*code++ != '{');
								break;
							}
							if(match(code, "ph::Camera"))
							{
								code += 10;
								fprintf(genFile, "ImGui::BulletText(\"%s\");\n", componentName);
								fprintf(genFile, "ImGui::Text(\"center: %%f, %%f\", c->center().x, c->center().y);\n");
								fprintf(genFile, "ImGui::Text(\"size: %%f, %%f\", c->getSize().x, c->getSize().y);\n");
								while(*code++ != '{');
								break;
							}

							// handle unknown parent type
							fprintf(genFile, "ImGui::BulletText(\"%s\"); // unknown parent type!\n", componentName);
							while(isAlpha(*code)) ++code;
							while(*code++ != '{');
							break;
						}
					}
				}
				if(componentUsesInheritance) break;

				++code;
			}

			Enums enums;

			while(*code)
			{	
				if(*code == '=')
				{
					// handle struct inline initialization
					while(*code != ';') ++code;
					++code;
				}

				auto parseEnum = [&](bool isEnumClass)
				{
					code += isEnumClass ? 10 : 4;
					while(!isAlpha(*code)) ++code;
					char* name = code;
					while(isAlpha(*code)) ++code;
					*code = 0;
					++code;
					auto* e = addEnum(&enums, name, isEnumClass);
					while(*code++)
					{
						if(isAlpha(*code))
						{	
							char* enumeration = code;
							while(isAlpha(*code)) ++code;
							*code = 0;
							++code;
							addEnumeration(e, enumeration);	
						}
						if(*code == ';')
						{
							++code;
							break;
						}
					}
				};

				auto parseVariableName = [&]()
				{
					while(!isAlpha(*code)) ++code;
					char* name = code;
					while(isAlpha(*code)) ++code;
					*code = 0;
					return name;
				};

				if(isAlpha(*code))
				{
					// TODO: What to do with PressurePlate which includes other component? 

					if(match(code, "ph::"))
					{
						code += 3;
					}
					else if(match(code, "struct"))
					{
						while(*code++)
						{
							if(match(code, "};")) break;
						}
					}
					else if(match(code, "inline"))
					{
						while(*code++ != ';'); 
					}
					else if(match(code, "static"))
					{
						while(*code++ != ';'); 
					}
					else if(match(code, "constexpr"))
					{
						code += 9;
					}
					else if(match(code, "const"))
					{
						code += 5;
					}
					else if(match(code, "using"))
					{
						code += 5;
						while(*code != ';') ++code;
					}
					else if(match(code, "enum class"))
					{
						parseEnum(true);
					}
					else if(match(code, "enum"))
					{
						parseEnum(false);
					}
					else if(match(code, "float"))
					{
						code += 5;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%f\", c->%s);\n", name, name);
					}
					else if(match(code, "int"))
					{
						code += 3;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%i\", c->%s);\n", name, name);
					}
					else if(match(code, "unsigned"))
					{
						code += 8;
						while(!isAlpha(*code)) ++code;
						if(match(code, "long long")) code += 9;
						else if(match(code, "long")) code += 4;
						else if(match(code, "int")) code += 3;
						else if(match(code, "short")) code += 5;
						else if(match(code, "char")) code += 4;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%u\", c->%s);\n", name, name);
					}
					else if(match(code, "bool"))
					{
						code += 4;
						char* name = parseVariableName();
						fprintf(genFile, "if(c->%s) ImGui::Text(\"%s: true\"); else ImGui::Text(\"%s: false\");\n", name, name, name);
					}
					else if(match(code, "char"))
					{
						code += 4;
						while(!isAlpha(*code)) ++code;
						char* name = code;
						while(isAlpha(*code)) ++code;
						char* nameEnd = code;
						while(*code)
						{
							if(*code == ';')
							{
								*nameEnd = 0;
								fprintf(genFile, "ImGui::Text(\"%s: %%c\", c->%s);\n", name, name);	
								break;
							}
							if(*code == '[')
							{
								*nameEnd = 0;
								fprintf(genFile, "ImGui::Text(\"%s: %%s\", c->%s);\n", name, name);
								break;
							}
							if(code - name > 50)
							{
								printf("Parsing error! (%u)", __LINE__);
								printf("Press enter!");
								getchar();
								return;
							}
							++code;
						}
					}
					else if(match(code, "FloatRect"))
					{
						code += 9;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%f, %%f, %%f, %%f\", c->%s.x,  c->%s.y, c->%s.w, c->%s.h);\n", name, name, name, name, name);
					}
					else if(match(code, "IntRect"))
					{
						code += 7;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%i, %%i, %%i, %%i\", c->%s.x,  c->%s.y, c->%s.w, c->%s.h);\n", name, name, name, name, name);
					}
					else if(match(code, "std::string"))
					{
						code += 11;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%s\", c->%s.c_str());\n", name, name);
					}
					else if(match(code, "sf::Vector2f"))
					{
						code += 12;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%f, %%f\", c->%s.x, c->%s.y);\n", name, name, name);
					}
					else if(match(code, "sf::Color"))
					{
						code += 9;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %%u, %%u, %%u, %%u\", c->%s.r, c->%s.g, c->%s.b, c->%s.a);\n", name, name, name, name, name);
					}
					else if(match(code, "std::"))
					{
						// handle not supported types from c++ std libary
						code += 5;
						char* typeName = code;
						while(isAlpha(*code)) ++code;
						char* typeNameEnd = code;
						while(*code != ' ') ++code;
						while(!isAlpha(*code)) ++code;
						char* name = code;
						while(isAlpha(*code)) ++code;
						*typeNameEnd = 0;
						*code = 0;
						fprintf(genFile, "ImGui::Text(\"%s: std::%s view is not supported!\");\n", name, typeName);
						++code;
					}
					else
					{
						// write local enums
						bool itWasEnum = false;
						for(u32 i = 0; i < enums.enumsUsed; ++i)
						{
							auto& e = enums.enums[i];
							if(match(code, e.name))
							{
								while(isAlpha(*code)) ++code;
								while(!isAlpha(*code)) ++code;
								char* varName = code;
								while(isAlpha(*code)) ++code;
								*code = 0;
								++code;
								fprintf(genFile, "switch(c->%s) {\n", varName);
								for(u32 enumerationIndex = 0;
								    enumerationIndex < e.nrOfEnumerations;
									++enumerationIndex)
								{
									char* enumeration = e.enumerations[enumerationIndex];
									if(e.isEnumClass)
									{
										fprintf(genFile, "case component::%s::%s::%s: ImGui::Text(\"%s: %s\"); break;\n", componentName, e.name, enumeration, varName, enumeration);
									}
									else
									{
										fprintf(genFile, "case component::%s::%s: ImGui::Text(\"%s: %s\"); break;\n", componentName, enumeration, varName, enumeration);
									}
								}
								fprintf(genFile, "default: ImGui::Text(\"%s: unknown enumeration!!!\");\n}\n", componentName);
								itWasEnum = true;
								break;
							}
						}
						if(itWasEnum) continue;

						{
							// check for pointers
							char* code2 = code;	
							while(isAlpha(*code2)) ++code2;
							bool itWasPointer = false; 
							while(!isAlpha(*code2))
							{
								if(*code2 == '*')
								{
									itWasPointer = true;
									while(!isAlpha(*code2)) ++code2;
									char* name = code2;
									while(isAlpha(*code2)) ++code2;
									*code2 = 0;
									fprintf(genFile, "ImGui::Text(\"%s: %%p\", c->%s);\n", name, name);
									code = code2 + 1;
									break;
								}
								++code2;
							}
							if(itWasPointer) continue;
						}

						// assume that it's unknown type
						char* typeName = code;
						while(isAlpha(*code)) ++code; 
						*code = 0;
						char* name = parseVariableName();
						fprintf(genFile, "ImGui::Text(\"%s: %s view is not supported!\");\n", name, typeName);
					}
				}
				else if(match(code, "};"))
				{
					code += 2;
					break;
				}

				++code;
			}

			fprintf(genFile, "}\n");
		}
		else if(match(code, "enum class"))
		{
			code += 10;
			while(!isAlpha(*code)) ++code;
			char* componentName = code;
			while(isAlpha(*code)) ++code;
			*code = 0;
			++code;
			fprintf(genFile, "if(auto* c = mRegistry.try_get<component::%s>(mSelected))\n{\n", componentName);
			fprintf(genFile, "ImGui::Separator();\n");
			fprintf(genFile, "switch(*c)\n{\n");

			code += 10;
			while(*code++)
			{
				if(*code == '}')
				{
					code += 2;
					break;
				}
				if(isAlpha(*code))
				{	
					char* enumeration = code;
					while(isAlpha(*code)) ++code; 
					*code = 0;
					++code;
					fprintf(genFile, "case component::%s::%s: ImGui::BulletText(\"%s: %s\"); break;\n", componentName, enumeration, componentName, enumeration);
				}
			}

			fprintf(genFile, "default: ImGui::BulletText(\"%s: unknown enumeration!!!\");\n}\n}\n", componentName);
		}
	}
	
	fclose(componentsFile);
}

int main()
{
	printf("components parser starts generating entities debugger!\n");

	FILE* genFile = fopen("../src/ECS/Systems/entitiesDebuggerGENERATED.cpp", "w");

	if(!genFile)
	{
		printf("Failed to open entitiesDebugger.cpp");
		return 0;
	}

	WIN32_FIND_DATA findResult;
	HANDLE findHandle = FindFirstFile("../src/ECS/Components/*", &findResult);
	if(findHandle == INVALID_HANDLE_VALUE)
	{
		printf("Failed to find first component file in ECS/Components directory!");
		return 0;
	}

	char componentFiles[50][60];
	u32 nrOfComponentFiles = 0;

	fprintf(genFile, "%s", genFileFirstHeader);

	do
	{
		if(strlen(findResult.cFileName) > 3)
		{
			fprintf(genFile, "#include \"ECS/Components/%s\"\n", findResult.cFileName);
			memcpy(componentFiles[nrOfComponentFiles], findResult.cFileName, strlen(findResult.cFileName) + 1);
			++nrOfComponentFiles;
		}
	}
	while(FindNextFile(findHandle, &findResult));
	FindClose(findHandle);

	fprintf(genFile, "%s", genFileSecondHeader);

	for(int i = 0; i < nrOfComponentFiles; ++i)
	{
		parseComponentsFile(componentFiles[i], genFile);
	}

	fprintf(genFile, "%s", genFileFooter);
	fclose(genFile);

	printf("components parser finished generating entities debugger!\n");

	return 0;
}
