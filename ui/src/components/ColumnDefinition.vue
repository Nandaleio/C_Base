<script lang="ts" setup>
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import {
    Select,
    SelectContent,
    SelectItem,
    SelectTrigger,
    SelectValue,
} from '@/components/ui/select'
import { Toggle } from '@/components/ui/toggle'
import {
    Popover,
    PopoverContent,
    PopoverTrigger,
} from '@/components/ui/popover'

import { Trash2, Settings, Ban, Key, ArrowUp10, Sparkle, Info } from 'lucide-vue-next'
import type { Cols } from '@/utils/types';
import {
  Tooltip,
  TooltipContent,
  TooltipProvider,
  TooltipTrigger,
} from '@/components/ui/tooltip'


defineProps<{
    col: Cols
}>();

const emit = defineEmits<{
    delete: []
}>();

</script>

<template>
    <Input type="text" name="tableName" class="rounded-e-none" placeholder="Column name" v-model="col.name" />
    <Select v-model="col.type">
        <SelectTrigger class="rounded-none">
            <SelectValue placeholder="Column type" />
        </SelectTrigger>
        <SelectContent>
            <SelectItem value="TEXT">Text</SelectItem>
            <SelectItem value="INTEGER">Integer</SelectItem>
            <SelectItem value="REAL">Number</SelectItem>
            <SelectItem value="DATE">Date</SelectItem>
            <SelectItem value="BLOB">File</SelectItem>
            <!-- <SelectItem disabled value="---">Relation</SelectItem> -->
        </SelectContent>
    </Select>
    <Popover>
        <PopoverTrigger>
            <Button variant="outline"  class="h-10 rounded-none">
                <Settings/>
            </Button>
        </PopoverTrigger>
        <PopoverContent >
            <div class="flex gap-3 items-center">
            <TooltipProvider>
                
                <Toggle variant="outline" v-bind:pressed="!!col.options.primary" @update:pressed="(v) => col.options.primary = v">
                    <Tooltip>
                        <TooltipTrigger as-child>
                                <Key />
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Primary Key</p>
                        </TooltipContent>
                    </Tooltip>
                </Toggle>

                <Toggle variant="outline" v-if="col.type === 'INTEGER' && !!col.options.primary" v-bind:pressed="!!col.options.autoincrement" @update:pressed="(v) => col.options.autoincrement = v">
                    <Tooltip>
                        <TooltipTrigger as-child>
                                <ArrowUp10 />
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Auto Increment</p>
                        </TooltipContent>
                    </Tooltip>
                </Toggle>

                <Toggle variant="outline" v-bind:pressed="!!col.options.notnull" @update:pressed="(v) => col.options.notnull = v">
                    <Tooltip>
                        <TooltipTrigger as-child>
                                <Ban />
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Not NULL</p>
                        </TooltipContent>
                    </Tooltip>
                </Toggle>

                <Toggle variant="outline" v-bind:pressed="!!col.options.unique" @update:pressed="(v) => col.options.unique = v">
                    <Tooltip>
                        <TooltipTrigger as-child>
                            <Sparkle />
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Unique</p>
                        </TooltipContent>
                    </Tooltip>
                </Toggle>

                <Input type="text" placeholder="Default value" v-model="col.options.default"/>

                <Tooltip>
                        <TooltipTrigger as-child>
                            <Input type="text" placeholder="Check expression" v-model="col.options.check"/>
                        </TooltipTrigger>
                        <TooltipContent>
                            <p>Check expression (do <b>NOT</b> write the column name)</p>
                        </TooltipContent>
                    </Tooltip>

                    <div class="w-5 h-5 flex items-center">
                        <Tooltip>
                            <TooltipTrigger as-child>
                                
                                <Info />
                            </TooltipTrigger>
                            <TooltipContent>
                                <p>Default value and check expression need to follow the <i><a target="_blank" href="https://www.sqlite.org/lang_createtable.html">SQLite column definition</a></i></p>
                            </TooltipContent>
                        </Tooltip>
                    </div>
                    
                
            </TooltipProvider>
            </div>
        </PopoverContent>
    </Popover>
    <Button variant="destructive" class="h-10 rounded-s-none" @click="emit('delete')">
        <Trash2 />
    </Button>

</template>